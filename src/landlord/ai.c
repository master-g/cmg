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

#include <stddef.h>

const ai_t ai_standard = {analysis_standard};

const ai_t ai_advanced = {analysis_advanced};

int ai_bid(const ai_view_t *view) {
  int shouldbid = 0;
  int handlistlen = 0;

  /* the fewer hands the cards need, the more they are worth */
  handlistlen = analysis_count_hands(analysis_standard, view->cards);

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
static const hand_t *ai_find_hand(const hand_list_t *hands, hand_type_t type) {
  int i = 0;

  for (i = 0; i < hand_list_count(hands); i++) {
    if (hand_type_equals(hand_list_at(hands, i)->type, type))
      return hand_list_at(hands, i);
  }

  return NULL;
}

static int ai_count_hands(const hand_list_t *hands, hand_type_t type) {
  int i = 0;
  int count = 0;

  for (i = 0; i < hand_list_count(hands); i++) {
    if (hand_type_equals(hand_list_at(hands, i)->type, type))
      count++;
  }

  return count;
}

static void ai_append_hand(hand_t *hand, const hand_t *part) {
  card_array_concat(&hand->cards, &part->cards);
}

void ai_lead(const ai_view_t *view, hand_t *hand) {
  const hand_list_t *hands = view->hands;
  int i = 0;
  const hand_t *node = NULL;
  int need = 0;
  const hand_type_t solo = hand_type(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, false);
  const hand_type_t pair = hand_type(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, false);
  const hand_type_t trio = hand_type(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, false);
  hand_type_t kickertype = solo;

  hand_clear(hand);

  /* empty hands */
  if ((hands == NULL) || (hand_list_count(hands) == 0))
    return;

  /* last hand */
  if (hand_list_count(hands) == 1) {
    ai_append_hand(hand, hand_list_at(hands, 0));
    return;
  }

  /* try to find longest hand combination */
  node =
      ai_find_hand(hands, hand_type(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, true));

  if (node != NULL) {
    ai_append_hand(hand, node);

    /* how many kickers do we need */
    need = card_array_length(&node->cards) / 3;

    /* trio-pair-chain then trio-solo-chain */
    if (ai_count_hands(hands, pair) >= need)
      kickertype = pair;
    else if (ai_count_hands(hands, solo) >= need)
      kickertype = solo;
    else
      return;

    for (i = 0; (need > 0) && (i < hand_list_count(hands)); i++) {
      if (hand_type_equals(hand_list_at(hands, i)->type, kickertype)) {
        ai_append_hand(hand, hand_list_at(hands, i));
        need--;
      }
    }

    return;
  }

  /* pair chain */
  node =
      ai_find_hand(hands, hand_type(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, true));

  /* solo chain */
  if (node == NULL)
    node = ai_find_hand(
        hands, hand_type(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, true));

  if (node != NULL) {
    ai_append_hand(hand, node);
    return;
  }

  /* trio */
  node = ai_find_hand(hands, trio);

  if ((node != NULL) && (hand_rank(node) != CARD_RANK_2)) {
    ai_append_hand(hand, node);

    /* pair */
    node = ai_find_hand(hands, pair);

    if ((node == NULL) || (hand_rank(node) == CARD_RANK_2)) {
      /* solo, 2 and jokers are too good to be thrown in as a kicker */
      node = ai_find_hand(hands, solo);

      if ((node != NULL) && (hand_rank(node) >= CARD_RANK_2))
        node = NULL;
    }

    /* no solo nor pair, return with trio */
    if (node != NULL)
      ai_append_hand(hand, node);

    return;
  }

  /* pair */
  node = ai_find_hand(hands, pair);

  if ((node != NULL) && (hand_rank(node) != CARD_RANK_2)) {
    ai_append_hand(hand, node);
    return;
  }

  /* just play */
  ai_append_hand(hand, hand_list_at(hands, 0));
}

#define AI_BEAT_VALUE_FACTOR 10

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
static int ai_best_beat(
    const card_array_t *cards, const hand_t *tobeat, hand_t *beat,
    analysis_func_t analyze) {
  hand_list_t beats;
  int i = 0;
  int normal = 0;
  int chosen = -1;
  int chosenvalue = 0;

  beat_search_all(cards, tobeat, &beats);

  for (i = 0; i < hand_list_count(&beats); i++) {
    const hand_t *candidate = hand_list_at(&beats, i);

    if (hand_is_bomb(candidate) || hand_is_nuke(candidate))
      chosen = i;
    else
      normal++;
  }

  if (chosen < 0) {
    for (i = 0; i < hand_list_count(&beats); i++) {
      const hand_t *candidate = hand_list_at(&beats, i);
      card_array_t rest;
      int value = 0;

      /* a single candidate needs no valuing */
      if (normal > 1) {
        card_array_copy(&rest, cards);
        card_array_subtract(&rest, &candidate->cards);
        value = analysis_count_hands(analyze, &rest) * AI_BEAT_VALUE_FACTOR +
                hand_rank(candidate);
      }

      if ((chosen < 0) || (value >= chosenvalue)) {
        chosen = i;
        chosenvalue = value;
      }
    }
  }

  if (chosen < 0)
    return 0;

  hand_copy(beat, hand_list_at(&beats, chosen));
  return 1;
}

int ai_beat(const ai_view_t *view, hand_t *hand) {
  int canbeat = 0;

  hand_clear(hand);

  canbeat = ai_best_beat(view->cards, view->last_hand, hand, view->ai->analyze);

  /* peasant cooperation: the last hand came from the other peasant */
  if (canbeat && (view->seat != view->landlord) &&
      (view->last_player != view->landlord)) {
    /* don't bomb/nuke teammate */
    if (hand_is_bomb(hand) || hand_is_nuke(hand))
      canbeat = 0;

    /* let the teammate run when it is closer to going out */
    if (view->cards_left[view->last_player] < view->cards_left[view->seat])
      canbeat = 0;
  }

  return canbeat;
}
