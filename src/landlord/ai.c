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
  card_array_t cards;
  rk_list_t *handlist = NULL;

  /* the fewer hands the cards need, the more they are worth */
  CardArray_Copy(&cards, view->cards);
  handlist = Analysis_Standard(&cards);
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

#define BEAT_NODE_CAPACITY 255

#define BEAT_VALUE_FACTOR 10

/* nodes for beat list sort */
typedef struct beat_node_s {
  hand_t *hand;
  int value;
  int order; /* search order, breaks ties so the sort is deterministic */

} beat_node_t;

/* sort function */
static int BeatNode_ValueSort(const void *a, const void *b) {
  const beat_node_t *na = *(beat_node_t *const *)a;
  const beat_node_t *nb = *(beat_node_t *const *)b;

  return na->value != nb->value ? na->value - nb->value : na->order - nb->order;
}

/* the beat that leaves the cards in the best shape, bombs come last */
static int AI_BestBeat(
    card_array_t *array, hand_t *tobeat, hand_t *beat, Analysis_Func analyze) {
  int i = 0;
  int nodei = 0;
  int bombi = 0;
  int canbeat = 0;
  rk_list_t *hl = NULL;
  rk_list_node_t *node = NULL;
  card_array_t temp;
  beat_node_t *hnodes[BEAT_NODE_CAPACITY];
  hand_t *hbombs[BEAT_NODE_CAPACITY];

  memset(hnodes, 0, sizeof(beat_node_t *) * BEAT_NODE_CAPACITY);
  memset(hbombs, 0, sizeof(hand_t *) * BEAT_NODE_CAPACITY);

  /* search beat list */
  hl = Beat_SearchAll(array, tobeat);

  /* separate bomb/nuke and normal hands */
  node = hl->first;

  while (node != NULL) {
    if (Hand_IsBomb(HandList_GetHand(node)) ||
        Hand_IsNuke(HandList_GetHand(node))) {
      hbombs[bombi++] = node->payload;
    } else {
      hnodes[nodei] = (beat_node_t *)malloc(sizeof(beat_node_t));
      hnodes[nodei]->hand = node->payload;
      hnodes[nodei]->order = nodei;
      nodei++;
    }

    node = node->next;
  }

  /* calculate value */
  if (nodei > 1) {
    for (i = 0; i < nodei; i++) {
      hand_t *leftover;
      CardArray_Copy(&temp, array);

      /* evaluate the value of cards left after hand was played */
      leftover = hnodes[i]->hand;
      CardArray_Subtract(&temp, &leftover->cards);

      hnodes[i]->value =
          Analysis_CountHands(analyze, &temp) * BEAT_VALUE_FACTOR +
          CARD_RANK(leftover->cards.cards[0]);
    }

    /* sort primal hands */
    qsort(hnodes, (size_t)nodei, sizeof(beat_node_t *), BeatNode_ValueSort);
  }

  /* re-build hand list */
  rk_list_destroy(hl);
  hl = rk_list_create();

  for (i = bombi; i >= 0; i--) {
    if (hbombs[i]) {
      rk_list_push(hl, hbombs[i]);
    }
  }

  for (i = nodei; i >= 0; i--) {
    if (hnodes[i]) {
      rk_list_push(hl, hnodes[i]->hand);
    }
  }

  /* select beat */
  if (!rk_list_empty(hl)) {
    Hand_Copy(beat, HandList_GetHand(hl->first));
    canbeat = 1;
  }

  /* clean up */
  for (i = 0; i < nodei; i++)
    free(hnodes[i]);

  rk_list_clear_destroy(hl);

  return canbeat;
}

int AI_Beat(const ai_view_t *view, hand_t *hand) {
  int canbeat = 0;
  card_array_t cards;
  hand_t tobeat;

  Hand_Clear(hand);
  CardArray_Copy(&cards, view->cards);
  Hand_Copy(&tobeat, view->lastHand);

  canbeat = AI_BestBeat(&cards, &tobeat, hand, view->ai->analyze);

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
