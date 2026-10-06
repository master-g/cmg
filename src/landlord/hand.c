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

/*
 * brief about Hand_Parse function
 * --------------------------------------
 *
 * A hand is recognised from how many cards of each rank it holds, the suits
 * never matter:
 *
 * 1. count every rank, and how many ranks appear once, twice, three and four
 *    times
 * 2. the ranks that appear most are the primal part (solo, pair, trio, four),
 *    two or more of them must be consecutive and below 2 to form a chain
 * 3. whatever is left must be exactly the kickers that primal part can carry
 */

#include "hand.h"

/* ************************************************************
 * hand
 * ************************************************************/
hand_type_t Hand_Type(hand_primal_t primal, hand_kicker_t kicker, bool chain) {
  hand_type_t type;

  type.primal = primal;
  type.kicker = kicker;
  type.chain = chain;

  return type;
}

bool Hand_TypeEquals(hand_type_t a, hand_type_t b) {
  return (a.primal == b.primal) && (a.kicker == b.kicker) &&
         (a.chain == b.chain);
}

bool Hand_IsType(
    const hand_t *hand, hand_primal_t primal, hand_kicker_t kicker,
    bool chain) {
  return Hand_TypeEquals(hand->type, Hand_Type(primal, kicker, chain));
}

bool Hand_IsNone(const hand_t *hand) {
  return hand->type.primal == HAND_PRIMAL_NONE;
}

void Hand_Clear(hand_t *hand) {
  CardArray_Clear(&hand->cards);
  hand->type = Hand_Type(HAND_PRIMAL_NONE, HAND_KICKER_NONE, false);
}

void Hand_Copy(hand_t *dst, const hand_t *src) {
  dst->type = src->type;
  CardArray_Copy(&dst->cards, &src->cards);
}

/* ************************************************************
 * parser
 * ************************************************************/

/* sorted cards, so the same card twice would be next to each other */
static int Hand_HasDuplicate(const card_array_t *sorted) {
  int i = 0;

  for (i = 1; i < CardArray_Length(sorted); i++) {
    if (CardArray_At(sorted, i) == CardArray_At(sorted, i - 1))
      return 1;
  }

  return 0;
}

/*
 * the ranks holding exactly `duplicate` cards can be played as the primal
 * part: either a single rank, or consecutive ranks below 2
 * | 666 | 777 | 888 | 999 |    duplicate: 3, ranks: 4
 */
static int Hand_IsPrimalRun(const int *count, int duplicate, int ranks) {
  int i = 0;
  int first = 0;
  int last = 0;

  for (i = CARD_RANK_BEG; i < CARD_RANK_END; i++) {
    if (count[i] == duplicate) {
      if (first == 0)
        first = i;
      last = i;
    }
  }

  if (ranks == 1)
    return 1;

  /* joker and 2 can't chain up */
  return (last < CARD_RANK_2) && (last - first + 1 == ranks);
}

/*
 * primal cards first, kickers after them, both from high to low
 * for example 88666644 becomes 66668844
 */
static void Hand_Arrange(
    hand_t *hand, const card_array_t *sorted, const int *count, int primal) {
  int rank = 0;

  /* sorted is high to low, so walk the ranks the same way */
  for (rank = CARD_RANK_END - 1; rank >= CARD_RANK_BEG; rank--) {
    if (count[rank] == primal)
      CardArray_CopyRank(&hand->cards, sorted, rank);
  }

  for (rank = CARD_RANK_END - 1; rank >= CARD_RANK_BEG; rank--) {
    if ((count[rank] != 0) && (count[rank] != primal))
      CardArray_CopyRank(&hand->cards, sorted, rank);
  }
}

bool Hand_Parse(hand_t *hand, const card_array_t *array) {
  int i = 0;
  int primal = 0;
  int ranks = 0;
  hand_kicker_t kicker = HAND_KICKER_NONE;
  int count[CARD_RANK_END];
  int times[HAND_PRIMAL_FOUR + 1] = {0}; /* times[n]: ranks appearing n times */
  int chainranks[HAND_PRIMAL_FOUR + 1] = {
      0, HAND_SOLO_CHAIN_MIN_LENGTH, HAND_PAIR_CHAIN_MIN_LENGTH / 2,
      HAND_TRIO_CHAIN_MIN_LENGTH / 3, HAND_FOUR_CHAIN_MIN_LENGTH / 4};
  card_array_t sorted;
  int length = CardArray_Length(array);

  Hand_Clear(hand);

  if ((CardArray_Length(array) < HAND_MIN_LENGTH) ||
      (CardArray_Length(array) > HAND_MAX_LENGTH))
    return false;

  CardArray_Copy(&sorted, array);
  CardArray_Sort(&sorted);

  if (Hand_HasDuplicate(&sorted))
    return false;

  CardArray_CountRanks(&sorted, count);

  for (i = CARD_RANK_BEG; i < CARD_RANK_END; i++) {
    /* more than four of a rank, these are not cards from one deck */
    if (count[i] > HAND_PRIMAL_FOUR)
      return false;

    times[count[i]]++;
  }

  /* nuke */
  if ((length == 2) && count[CARD_RANK_r] && count[CARD_RANK_R]) {
    hand->type = Hand_Type(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, false);
    CardArray_Copy(&hand->cards, &sorted);
    return true;
  }

  /* bomb */
  if ((length == 4) && (times[4] == 1)) {
    hand->type = Hand_Type(HAND_PRIMAL_BOMB, HAND_KICKER_NONE, false);
    CardArray_Copy(&hand->cards, &sorted);
    return true;
  }

  /* the ranks that appear most are the primal part */
  for (primal = HAND_PRIMAL_FOUR; times[primal] == 0; primal--)
    ;
  ranks = times[primal];

  if (!Hand_IsPrimalRun(count, primal, ranks))
    return false;

  if (length == primal * ranks) {
    /* nothing but the primal part: solo, pair, trio or a chain of them */
    if ((ranks > 1) && (ranks < chainranks[primal]))
      return false;
  } else if (primal == HAND_PRIMAL_TRIO) {
    /* every trio carries one solo, or every trio carries one pair */
    if ((times[1] == ranks) && (times[2] == 0))
      kicker = HAND_KICKER_SOLO;
    else if ((times[2] == ranks) && (times[1] == 0))
      kicker = HAND_KICKER_PAIR;
    else
      return false;
  } else if (primal == HAND_PRIMAL_FOUR) {
    /* every four carries two solos, or every four carries two pairs */
    if ((times[1] == ranks * 2) && (times[2] == 0) && (times[3] == 0))
      kicker = HAND_KICKER_DUAL_SOLO;
    else if ((times[2] == ranks * 2) && (times[1] == 0) && (times[3] == 0))
      kicker = HAND_KICKER_DUAL_PAIR;
    else
      return false;
  } else {
    return false;
  }

  /* SOLO to FOUR are the number of cards per rank */
  hand->type = Hand_Type((hand_primal_t)primal, kicker, ranks > 1);
  Hand_Arrange(hand, &sorted, count, primal);

  return true;
}

/*
 * ************************************************************
 * layout: what Hand_Parse arranged
 * ************************************************************
 */

int Hand_Rank(const hand_t *hand) {
  return CARD_RANK(CardArray_At(&hand->cards, 0));
}

void Hand_Split(
    const hand_t *hand, card_array_t *primal, card_array_t *kickers) {
  int count[CARD_RANK_END];
  int rank = 0;
  int most = 0;

  CardArray_Clear(primal);
  CardArray_Clear(kickers);
  CardArray_CountRanks(&hand->cards, count);

  for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
    if (count[rank] > most)
      most = count[rank];
  }

  /* keep the order of the hand: high to low */
  for (rank = CARD_RANK_END - 1; rank >= CARD_RANK_BEG; rank--) {
    if (count[rank] == 0)
      continue;

    CardArray_CopyRank(
        count[rank] == most ? primal : kickers, &hand->cards, rank);
  }
}

/*
 * ************************************************************
 * comparators
 * ************************************************************
 */

bool Hand_IsBomb(const hand_t *hand) {
  return Hand_IsType(hand, HAND_PRIMAL_BOMB, HAND_KICKER_NONE, false);
}

bool Hand_IsNuke(const hand_t *hand) {
  return Hand_IsType(hand, HAND_PRIMAL_NUKE, HAND_KICKER_NONE, false);
}

HandCompareResult Hand_Compare(const hand_t *a, const hand_t *b) {
  if (!Hand_TypeEquals(a->type, b->type)) {
    /* different hand types only compare when a bomb or the nuke is in */
    if (!Hand_IsBomb(a) && !Hand_IsNuke(a) && !Hand_IsBomb(b) &&
        !Hand_IsNuke(b))
      return HAND_CMP_ILLEGAL;

    /* nuke over bomb over everything else */
    return a->type.primal > b->type.primal ? HAND_CMP_GREATER : HAND_CMP_LESS;
  }

  /* same hand type but different length */
  if (CardArray_Length(&a->cards) != CardArray_Length(&b->cards))
    return HAND_CMP_ILLEGAL;

  /* same hand type and same length, the rank decides */
  if (Hand_Rank(a) == Hand_Rank(b))
    return HAND_CMP_EQUAL;

  return Hand_Rank(a) > Hand_Rank(b) ? HAND_CMP_GREATER : HAND_CMP_LESS;
}

void Hand_Print(const hand_t *hand) {
  const char *toprint = "";

  DBGLog("Hand type: [");

  if (hand == NULL) {
    DBGLog("null\n");
    return;
  }

  switch (hand->type.primal) {
  case HAND_PRIMAL_NONE:
    toprint = "none ";
    break;

  case HAND_PRIMAL_SOLO:
    toprint = "solo ";
    break;

  case HAND_PRIMAL_PAIR:
    toprint = "pair ";
    break;

  case HAND_PRIMAL_TRIO:
    toprint = "trio ";
    break;

  case HAND_PRIMAL_FOUR:
    toprint = "four ";
    break;

  case HAND_PRIMAL_BOMB:
    toprint = "bomb ";
    break;

  case HAND_PRIMAL_NUKE:
    toprint = "nuke ";
    break;
  }

  DBGLog("%s", toprint);

  switch (hand->type.kicker) {
  case HAND_KICKER_NONE:
    toprint = NULL;
    break;

  case HAND_KICKER_SOLO:
    toprint = "solo";
    break;

  case HAND_KICKER_PAIR:
    toprint = "pair";
    break;

  case HAND_KICKER_DUAL_SOLO:
    toprint = "dual solo";
    break;

  case HAND_KICKER_DUAL_PAIR:
    toprint = "dual pair";
    break;
  }

  if (toprint != NULL)
    DBGLog("%s", toprint);

  if (hand->type.chain)
    DBGLog(" chain");

  DBGLog("]\n");

  CardArray_Print(&hand->cards);
}
