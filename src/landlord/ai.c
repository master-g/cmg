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

const ai_t AI_Standard = {HandList_StandardAnalyze, HandList_StandardEvaluator};

const ai_t AI_Advanced = {HandList_AdvancedAnalyze, HandList_AdvancedEvaluator};

int AI_Bid(const ai_view_t *view) {
  int shouldbid = 0;
  int handlistlen = 0;
  card_array_t cards;
  rk_list_t *handlist = NULL;

  /* the fewer hands the cards need, the more they are worth */
  CardArray_Copy(&cards, view->cards);
  handlist = HandList_StandardAnalyze(&cards);
  handlistlen = rk_list_count(handlist);
  rk_list_clear_destroy(handlist);

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
static const hand_t *AI_FindHand(const rk_list_t *hands, int type) {
  const rk_list_node_t *node = NULL;

  for (node = hands->first; node != NULL; node = node->next) {
    if (HandList_GetHand(node)->type == type)
      return HandList_GetHand(node);
  }

  return NULL;
}

static int AI_CountHands(const rk_list_t *hands, int type) {
  const rk_list_node_t *node = NULL;
  int count = 0;

  for (node = hands->first; node != NULL; node = node->next) {
    if (HandList_GetHand(node)->type == type)
      count++;
  }

  return count;
}

static void AI_AppendHand(hand_t *hand, const hand_t *part) {
  card_array_t cards;

  CardArray_Copy(&cards, &part->cards);
  CardArray_Concat(&hand->cards, &cards);
}

void AI_Lead(const ai_view_t *view, hand_t *hand) {
  const rk_list_t *hands = view->hands;
  const rk_list_node_t *temp = NULL;
  const hand_t *node = NULL;
  int need = 0;
  int kickertype = 0;

  Hand_Clear(hand);

  /* empty hands */
  if ((hands == NULL) || rk_list_empty(hands))
    return;

  /* last hand */
  if (rk_list_count(hands) == 1) {
    AI_AppendHand(hand, HandList_GetHand(hands->first));
    return;
  }

  /* try to find longest hand combination */
  node = AI_FindHand(
      hands, Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAIN));

  if (node != NULL) {
    AI_AppendHand(hand, node);

    /* how many kickers do we need */
    need = node->cards.length / 3;

    /* trio-pair-chain then trio-solo-chain */
    if (AI_CountHands(hands, HAND_PRIMAL_PAIR) >= need)
      kickertype = HAND_PRIMAL_PAIR;
    else if (AI_CountHands(hands, HAND_PRIMAL_SOLO) >= need)
      kickertype = HAND_PRIMAL_SOLO;
    else
      return;

    for (temp = hands->first; need > 0 && temp != NULL; temp = temp->next) {
      if (HandList_GetHand(temp)->type == kickertype) {
        AI_AppendHand(hand, HandList_GetHand(temp));
        need--;
      }
    }

    return;
  }

  /* pair chain */
  node = AI_FindHand(
      hands, Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAIN));

  /* solo chain */
  if (node == NULL)
    node = AI_FindHand(
        hands, Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAIN));

  if (node != NULL) {
    AI_AppendHand(hand, node);
    return;
  }

  /* trio */
  node = AI_FindHand(hands, HAND_PRIMAL_TRIO);

  if ((node != NULL) && (CARD_RANK(node->cards.cards[0]) != CARD_RANK_2)) {
    AI_AppendHand(hand, node);

    /* pair */
    node = AI_FindHand(hands, HAND_PRIMAL_PAIR);

    if ((node == NULL) || (CARD_RANK(node->cards.cards[0]) == CARD_RANK_2)) {
      /* solo, 2 and jokers are too good to be thrown in as a kicker */
      node = AI_FindHand(hands, HAND_PRIMAL_SOLO);

      if ((node != NULL) && (CARD_RANK(node->cards.cards[0]) >= CARD_RANK_2))
        node = NULL;
    }

    /* no solo nor pair, return with trio */
    if (node != NULL)
      AI_AppendHand(hand, node);

    return;
  }

  /* pair */
  node = AI_FindHand(hands, HAND_PRIMAL_PAIR);

  if ((node != NULL) && (CARD_RANK(node->cards.cards[0]) != CARD_RANK_2)) {
    AI_AppendHand(hand, node);
    return;
  }

  /* just play */
  AI_AppendHand(hand, HandList_GetHand(hands->first));
}

int AI_Beat(const ai_view_t *view, hand_t *hand) {
  int canbeat = 0;
  card_array_t cards;
  hand_t tobeat;

  Hand_Clear(hand);
  CardArray_Copy(&cards, view->cards);
  Hand_Copy(&tobeat, view->lastHand);

  canbeat = HandList_BestBeat(&cards, &tobeat, hand, view->ai->evaluate);

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
