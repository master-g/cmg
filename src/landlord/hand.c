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
void Hand_Clear(hand_t *hand) {
  CardArray_Clear(&hand->cards);
  hand->type = 0;
}

void Hand_Copy(hand_t *dst, const hand_t *src) {
  dst->type = src->type;
  CardArray_Copy(&dst->cards, &src->cards);
}

/* ************************************************************
 * parser
 * ************************************************************/

/*
 * count rank in card array
 * count[rank] = num
 */
void Hand_CountRank(card_array_t *array, int *count) {
  int i = 0;

  memset(count, 0, sizeof(int) * CARD_RANK_END);

  for (i = 0; i < array->length; i++)
    count[CARD_RANK(array->cards[i])]++;
}

/* sorted cards, so the same card twice would be next to each other */
static int Hand_HasDuplicate(const card_array_t *sorted) {
  int i = 0;

  for (i = 1; i < sorted->length; i++) {
    if (sorted->cards[i] == sorted->cards[i - 1])
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
  int i = 0;
  card_array_t kickers;

  CardArray_Clear(&kickers);

  for (i = 0; i < sorted->length; i++) {
    if (count[CARD_RANK(sorted->cards[i])] == primal)
      CardArray_PushBack(&hand->cards, sorted->cards[i]);
    else
      CardArray_PushBack(&kickers, sorted->cards[i]);
  }

  CardArray_Concat(&hand->cards, &kickers);
}

int Hand_Parse(hand_t *hand, const card_array_t *array) {
  int i = 0;
  int primal = 0;
  int ranks = 0;
  int kicker = HAND_KICKER_NONE;
  int count[CARD_RANK_END];
  int times[HAND_PRIMAL_FOUR + 1] = {0}; /* times[n]: ranks appearing n times */
  int chainranks[HAND_PRIMAL_FOUR + 1] = {
      0, HAND_SOLO_CHAIN_MIN_LENGTH, HAND_PAIR_CHAIN_MIN_LENGTH / 2,
      HAND_TRIO_CHAIN_MIN_LENGTH / 3, HAND_FOUR_CHAIN_MIN_LENGTH / 4};
  card_array_t sorted;

  Hand_Clear(hand);

  if ((array->length < HAND_MIN_LENGTH) || (array->length > HAND_MAX_LENGTH))
    return HAND_NONE;

  CardArray_Copy(&sorted, array);
  CardArray_Sort(&sorted, NULL);

  if (Hand_HasDuplicate(&sorted))
    return HAND_NONE;

  Hand_CountRank(&sorted, count);

  for (i = CARD_RANK_BEG; i < CARD_RANK_END; i++) {
    /* more than four of a rank, these are not cards from one deck */
    if (count[i] > HAND_PRIMAL_FOUR)
      return HAND_NONE;

    times[count[i]]++;
  }

  /* nuke */
  if ((sorted.length == 2) && count[CARD_RANK_r] && count[CARD_RANK_R]) {
    hand->type =
        Hand_Format(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, HAND_CHAINLESS);
    CardArray_Copy(&hand->cards, &sorted);
    return hand->type;
  }

  /* bomb */
  if ((sorted.length == 4) && (times[4] == 1)) {
    hand->type =
        Hand_Format(HAND_PRIMAL_BOMB, HAND_KICKER_NONE, HAND_CHAINLESS);
    CardArray_Copy(&hand->cards, &sorted);
    return hand->type;
  }

  /* the ranks that appear most are the primal part */
  for (primal = HAND_PRIMAL_FOUR; times[primal] == 0; primal--)
    ;
  ranks = times[primal];

  if (!Hand_IsPrimalRun(count, primal, ranks))
    return HAND_NONE;

  if (sorted.length == primal * ranks) {
    /* nothing but the primal part: solo, pair, trio or a chain of them */
    if ((ranks > 1) && (ranks < chainranks[primal]))
      return HAND_NONE;
  } else if (primal == HAND_PRIMAL_TRIO) {
    /* every trio carries one solo, or every trio carries one pair */
    if ((times[1] == ranks) && (times[2] == 0))
      kicker = HAND_KICKER_SOLO;
    else if ((times[2] == ranks) && (times[1] == 0))
      kicker = HAND_KICKER_PAIR;
    else
      return HAND_NONE;
  } else if (primal == HAND_PRIMAL_FOUR) {
    /* every four carries two solos, or every four carries two pairs */
    if ((times[1] == ranks * 2) && (times[2] == 0) && (times[3] == 0))
      kicker = HAND_KICKER_DUAL_SOLO;
    else if ((times[2] == ranks * 2) && (times[1] == 0) && (times[3] == 0))
      kicker = HAND_KICKER_DUAL_PAIR;
    else
      return HAND_NONE;
  } else {
    return HAND_NONE;
  }

  hand->type =
      Hand_Format(primal, kicker, ranks > 1 ? HAND_CHAIN : HAND_CHAINLESS);
  Hand_Arrange(hand, &sorted, count, primal);

  return hand->type;
}

/*
 * ************************************************************
 * comparators
 * ************************************************************
 */

/* one of a, b must be bomb or nuke */
static int Hand_CompareBomb(hand_t *a, hand_t *b) {
  int ret = HAND_CMP_ILLEGAL;

  /* same type same cards, equal */
  if ((a->type == b->type) && (a->cards.cards[0] == b->cards.cards[0]))
    ret = HAND_CMP_EQUAL;

  /* both are bombs, compare by card rank */
  else if ((a->type == HAND_PRIMAL_BOMB) && (b->type == HAND_PRIMAL_BOMB))
    ret = CARD_RANK(a->cards.cards[0]) > CARD_RANK(b->cards.cards[0])
              ? HAND_CMP_GREATER
              : HAND_CMP_LESS;
  else
    ret = Hand_GetPrimal(a->type) > Hand_GetPrimal(b->type) ? HAND_CMP_GREATER
                                                            : HAND_CMP_LESS;

  return ret;
}

int Hand_IsBomb(const hand_t *hand) {
  return hand->type ==
         Hand_Format(HAND_PRIMAL_BOMB, HAND_KICKER_NONE, HAND_CHAINLESS);
}

int Hand_IsNuke(const hand_t *hand) {
  return hand->type ==
         Hand_Format(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, HAND_CHAINLESS);
}

int Hand_Compare(hand_t *a, hand_t *b) {
  int result = HAND_CMP_ILLEGAL;

  /*
   * different hand type
   * check for bomb and nuke
   */
  if (a->type != b->type) {
    if ((a->type != HAND_PRIMAL_NUKE) && (a->type != HAND_PRIMAL_BOMB) &&
        (b->type != HAND_PRIMAL_NUKE) && (b->type != HAND_PRIMAL_BOMB))
      result = HAND_CMP_ILLEGAL;
    else
      result = Hand_CompareBomb(a, b);
  } else /* same hand type and with no bombs */
  {
    /* same hand type but different length */
    if (a->cards.length != b->cards.length) {
      result = HAND_CMP_ILLEGAL;
    } else /* same hand type and same length */
    {
      if (CARD_RANK(a->cards.cards[0]) == CARD_RANK(b->cards.cards[0]))
        result = HAND_CMP_EQUAL;
      else
        result = CARD_RANK(a->cards.cards[0]) > CARD_RANK(b->cards.cards[0])
                     ? HAND_CMP_GREATER
                     : HAND_CMP_LESS;
    }
  }

  return result;
}

void Hand_Print(hand_t *hand) {
  const char *toprint = "";

  DBGLog("Hand type: [");

  if (hand == NULL) {
    DBGLog("null\n");
    return;
  }

  switch (Hand_GetPrimal(hand->type)) {
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

  default:
    break;
  }

  DBGLog("%s", toprint);

  switch (Hand_GetKicker(hand->type)) {
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

  default:
    toprint = NULL;
    break;
  }

  if (toprint != NULL)
    DBGLog("%s", toprint);

  if (Hand_GetChain(hand->type) == HAND_CHAIN)
    DBGLog(" chain");

  DBGLog("]\n");

  CardArray_Print(&hand->cards);
}
