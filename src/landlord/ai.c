/*
The MIT License (MIT)

Copyright (c) 2014 Master.G

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ai.h"

const ai_t AI_Standard = {Analysis_Standard};

const ai_t AI_Advanced = {Analysis_Advanced};

int AI_Bid(const ai_view_t *view) {
  int shouldbid = 0;
  int handlistlen = 0;

  /* the fewer hands the cards need, the more they are worth */
  handlistlen = Analysis_CountHands(Analysis_Standard, view->cards);

  if (handlistlen > 9) {
    shouldbid = 0;
  } else if ((handlistlen < 9) && (handlistlen > 3)) {
    shouldbid = 1;
  } else if ((handlistlen <= 3) && (handlistlen > 2)) {
    shouldbid = 2;
  } else if (handlistlen <= 2) {
    shouldbid = 3;
  }

  if (shouldbid > view->bid)
    return shouldbid;
  else
    return 0;
}

/* first hand of that type the seat holds */
static const hand_t *AI_FindHand(const hand_list_t *hands, hand_type_t type) {
  int i = 0;

  for (i = 0; i < HandList_Count(hands); i++) {
    if (Hand_TypeEquals(HandList_At(hands, i)->type, type))
      return HandList_At(hands, i);
  }

  return NULL;
}

static int AI_CountHands(const hand_list_t *hands, hand_type_t type) {
  int i = 0;
  int count = 0;

  for (i = 0; i < HandList_Count(hands); i++) {
    if (Hand_TypeEquals(HandList_At(hands, i)->type, type))
      count++;
  }

  return count;
}

static void AI_AppendHand(hand_t *hand, const hand_t *part) {
  CardArray_Concat(&hand->cards, &part->cards);
}

void AI_Lead(const ai_view_t *view, hand_t *hand) {
  const hand_list_t *hands = view->hands;
  int i = 0;
  const hand_t *node = NULL;
  int need = 0;
  const hand_type_t solo = Hand_Type(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, false);
  const hand_type_t pair = Hand_Type(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, false);
  const hand_type_t trio = Hand_Type(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, false);
  hand_type_t kickertype = solo;

  Hand_Clear(hand);

  /* empty hands */
  if ((hands == NULL) || (HandList_Count(hands) == 0))
    return;

  /* last hand */
  if (HandList_Count(hands) == 1) {
    AI_AppendHand(hand, HandList_At(hands, 0));
    return;
  }

  /* try to find longest hand combination */
  node =
      AI_FindHand(hands, Hand_Type(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, true));

  if (node != NULL) {
    AI_AppendHand(hand, node);

    /* how many kickers do we need */
    need = CardArray_Length(&node->cards) / 3;

    /* trio-pair-chain then trio-solo-chain */
    if (AI_CountHands(hands, pair) >= need)
      kickertype = pair;
    else if (AI_CountHands(hands, solo) >= need)
      kickertype = solo;
    else
      return;

    for (i = 0; (need > 0) && (i < HandList_Count(hands)); i++) {
      if (Hand_TypeEquals(HandList_At(hands, i)->type, kickertype)) {
        AI_AppendHand(hand, HandList_At(hands, i));
        need--;
      }
    }

    return;
  }

  /* pair chain */
  node =
      AI_FindHand(hands, Hand_Type(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, true));

  /* solo chain */
  if (node == NULL)
    node =
        AI_FindHand(hands, Hand_Type(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, true));

  if (node != NULL) {
    AI_AppendHand(hand, node);
    return;
  }

  /* trio */
  node = AI_FindHand(hands, trio);

  if ((node != NULL) && (Hand_Rank(node) != CARD_RANK_2)) {
    AI_AppendHand(hand, node);

    /* pair */
    node = AI_FindHand(hands, pair);

    if ((node == NULL) || (Hand_Rank(node) == CARD_RANK_2)) {
      /* solo, 2 and jokers are too good to be thrown in as a kicker */
      node = AI_FindHand(hands, solo);

      if ((node != NULL) && (Hand_Rank(node) >= CARD_RANK_2))
        node = NULL;
    }

    /* no solo nor pair, return with trio */
    if (node != NULL)
      AI_AppendHand(hand, node);

    return;
  }

  /* pair */
  node = AI_FindHand(hands, pair);

  if ((node != NULL) && (Hand_Rank(node) != CARD_RANK_2)) {
    AI_AppendHand(hand, node);
    return;
  }

  /* just play */
  AI_AppendHand(hand, HandList_At(hands, 0));
}

#define BEAT_VALUE_FACTOR 10

/*
 * Choose one of the hands that beat tobeat.
 *
 * A bomb or the nuke is taken whenever there is one, the last one found.
 * Otherwise every candidate is valued by the hands the remaining cards would
 * take (times ten) plus its own rank, and the highest value is played.
 *
 * NOTE: this is what the AI has always done and the baseline records it, but
 * it reads backwards: it spends bombs first and keeps the split that needs
 * the most hands. Changing it is a change of strategy, not a refactoring.
 */
static int AI_BestBeat(
    const card_array_t *cards, const hand_t *tobeat, hand_t *beat,
    Analysis_Func analyze) {
  hand_list_t beats;
  int i = 0;
  int normal = 0;
  int chosen = -1;
  int chosenvalue = 0;

  Beat_SearchAll(cards, tobeat, &beats);

  for (i = 0; i < HandList_Count(&beats); i++) {
    const hand_t *candidate = HandList_At(&beats, i);

    if (Hand_IsBomb(candidate) || Hand_IsNuke(candidate))
      chosen = i;
    else
      normal++;
  }

  if (chosen < 0) {
    for (i = 0; i < HandList_Count(&beats); i++) {
      const hand_t *candidate = HandList_At(&beats, i);
      card_array_t rest;
      int value = 0;

      /* a single candidate needs no valuing */
      if (normal > 1) {
        CardArray_Copy(&rest, cards);
        CardArray_Subtract(&rest, &candidate->cards);
        value = Analysis_CountHands(analyze, &rest) * BEAT_VALUE_FACTOR +
                Hand_Rank(candidate);
      }

      if ((chosen < 0) || (value >= chosenvalue)) {
        chosen = i;
        chosenvalue = value;
      }
    }
  }

  if (chosen < 0)
    return 0;

  Hand_Copy(beat, HandList_At(&beats, chosen));
  return 1;
}

int AI_Beat(const ai_view_t *view, hand_t *hand) {
  int canbeat = 0;

  Hand_Clear(hand);

  canbeat = AI_BestBeat(view->cards, view->lastHand, hand, view->ai->analyze);

  /* peasant cooperation: the last hand came from the other peasant */
  if (canbeat && (view->seat != view->landlord) &&
      (view->lastPlayer != view->landlord)) {
    /* don't bomb/nuke teammate */
    if (Hand_IsBomb(hand) || Hand_IsNuke(hand))
      canbeat = 0;

    /* let the teammate run when it is closer to going out */
    if (view->cardsLeft[view->lastPlayer] < view->cardsLeft[view->seat])
      canbeat = 0;
  }

  return canbeat;
}
