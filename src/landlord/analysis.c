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

#include "analysis.h"
#include "beat.h"

#include <limits.h>

/*
 * extract hands like 34567 / 334455 / 333444555 etc
 * from the ranks held exactly `duplicate` times: consecutive ranks become one
 * chain when there are enough of them, single hands otherwise
 * cards has to be sorted from high to low
 */
static void HandList_ExtractConsecutive(
    hand_list_t *hl, const card_array_t *cards, const int *count,
    int duplicate) {
  int rank = 0;
  int runtop = 0; /* highest rank of the run being collected */
  int runlen = 0; /* ranks in that run */
  hand_t hand;
  int primal[] = {0, HAND_PRIMAL_SOLO, HAND_PRIMAL_PAIR, HAND_PRIMAL_TRIO};
  int chainlen[] = {
      0, HAND_SOLO_CHAIN_MIN_LENGTH, HAND_PAIR_CHAIN_MIN_LENGTH,
      HAND_TRIO_CHAIN_MIN_LENGTH};

  if ((duplicate < 1) || (duplicate > 3))
    return;

  /* 2 and jokers never chain, one step below the lowest rank ends the run */
  for (rank = CARD_RANK_2 - 1; rank >= CARD_RANK_BEG - 1; rank--) {
    int r = 0;

    if ((rank >= CARD_RANK_BEG) && (count[rank] == duplicate)) {
      if (runlen == 0)
        runtop = rank;

      runlen++;
      continue;
    }

    if (runlen * duplicate >= chainlen[duplicate]) {
      /* chain */
      Hand_Clear(&hand);
      hand.type = Hand_Format(primal[duplicate], HAND_KICKER_NONE, HAND_CHAIN);

      for (r = runtop; r > runtop - runlen; r--)
        CardArray_CopyRank(&hand.cards, cards, (uint8_t)r);

      HandList_Push(hl, &hand);
    } else {
      /* not a chain */
      for (r = runtop; r > runtop - runlen; r--) {
        Hand_Clear(&hand);
        hand.type =
            Hand_Format(primal[duplicate], HAND_KICKER_NONE, HAND_CHAINLESS);
        CardArray_CopyRank(&hand.cards, cards, (uint8_t)r);
        HandList_Push(hl, &hand);
      }
    }

    runlen = 0;
  }
}

/* extract nuke/bomb/2 from array, these cards will be removed from array */
static void
HandList_ExtractNukeBomb2(hand_list_t *hl, card_array_t *array, int *count) {
  int i = 0;
  hand_t hand;

  /* nuke */
  if (count[CARD_RANK_r] && count[CARD_RANK_R]) {
    Hand_Clear(&hand);
    hand.type = Hand_Format(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, HAND_CHAINLESS);
    CardArray_CopyRank(&hand.cards, array, CARD_RANK_R);
    CardArray_CopyRank(&hand.cards, array, CARD_RANK_r);

    HandList_Push(hl, &hand);

    count[CARD_RANK_r] = 0;
    count[CARD_RANK_R] = 0;

    CardArray_RemoveRank(array, CARD_RANK_r);
    CardArray_RemoveRank(array, CARD_RANK_R);
  }

  /* bomb */
  for (i = CARD_RANK_2; i >= CARD_RANK_3; i--) {
    if (count[i] == 4) {
      Hand_Clear(&hand);
      hand.type =
          Hand_Format(HAND_PRIMAL_BOMB, HAND_KICKER_NONE, HAND_CHAINLESS);
      CardArray_CopyRank(&hand.cards, array, (uint8_t)i);

      HandList_Push(hl, &hand);

      count[i] = 0;
      CardArray_RemoveRank(array, (uint8_t)i);
    }
  }

  /* joker */
  if ((count[CARD_RANK_r] != 0) || (count[CARD_RANK_R] != 0)) {
    Hand_Clear(&hand);
    CardArray_CopyRank(
        &hand.cards, array,
        count[CARD_RANK_r] != 0 ? CARD_RANK_r : CARD_RANK_R);
    hand.type = Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAINLESS);

    HandList_Push(hl, &hand);
    count[CARD_RANK_r] = 0;
    count[CARD_RANK_R] = 0;
    CardArray_RemoveRank(array, CARD_RANK_r);
    CardArray_RemoveRank(array, CARD_RANK_R);
  }

  /* 2 */
  if (count[CARD_RANK_2] != 0) {
    Hand_Clear(&hand);
    CardArray_CopyRank(&hand.cards, array, CARD_RANK_2);

    switch (count[CARD_RANK_2]) {
    case 1:
      hand.type =
          Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAINLESS);
      break;

    case 2:
      hand.type =
          Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAINLESS);
      break;

    case 3:
      hand.type =
          Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAINLESS);
      break;

    default:
      break;
    }
    count[CARD_RANK_2] = 0;
    CardArray_RemoveRank(array, CARD_RANK_2);
    HandList_Push(hl, &hand);
  }
}

void Analysis_Standard(const card_array_t *cards, hand_list_t *hl) {
  int count[CARD_RANK_END];
  card_array_t array;

  CardArray_Copy(&array, cards);
  CardArray_Sort(&array);
  CardArray_CountRanks(&array, count);

  HandList_Clear(hl);

  /* nuke, bomb and 2 */
  HandList_ExtractNukeBomb2(hl, &array, count);

  /* trios, pairs and solos, chained up where they can */
  HandList_ExtractConsecutive(hl, &array, count, 3);
  HandList_ExtractConsecutive(hl, &array, count, 2);
  HandList_ExtractConsecutive(hl, &array, count, 1);
}

int Analysis_CountHands(Analysis_Func analyze, const card_array_t *array) {
  hand_list_t hl;

  analyze(array, &hl);

  return HandList_Count(&hl);
}

/*
 * ************************************************************
 * advanced analysis
 * ************************************************************
 */

/* cards being taken apart */
typedef struct hand_ctx_s {
  /* rank count */
  int count[CARD_RANK_END];
  /* original cards */
  card_array_t cards;
  /* the cards, low to high */
  card_array_t rcards;

} hand_ctx_t;

#define HandCtx_Clear(ctx) memset((ctx), 0, sizeof(hand_ctx_t))

/* the longest chain of ranks held at least `duplicate` times, lowest wins */
static void HandList_SearchLongestConsecutive(
    hand_ctx_t *ctx, hand_t *hand, int duplicate) {
  int i = 0;
  int rankstart = 0;
  int beststart = 0;
  int bestlen = 0;
  int primal[] = {0, HAND_PRIMAL_SOLO, HAND_PRIMAL_PAIR, HAND_PRIMAL_TRIO};
  int chainlen[] = {
      0, HAND_SOLO_CHAIN_MIN_LENGTH, HAND_PAIR_CHAIN_MIN_LENGTH,
      HAND_TRIO_CHAIN_MIN_LENGTH};
  int *count = ctx->count;

  if ((duplicate < 1) || (duplicate > 3))
    return;

  /* early break */
  if (CardArray_Length(&ctx->rcards) < chainlen[duplicate])
    return;

  Hand_Clear(hand);

  /*
   * i <= CARD_RANK_2
   * but count[CARD_RANK_2] must be 0
   * for 2/bomb/nuke has been removed before calling this function
   */
  for (i = CARD_RANK_3; i <= CARD_RANK_2; i++) {
    /* find start of a possible chain */
    if (rankstart == 0) {
      if (count[i] >= duplicate)
        rankstart = i;

      continue;
    }

    /* chain break, keep it when it is a chain and the longest so far */
    if (count[i] < duplicate) {
      if ((((i - rankstart) * duplicate) >= chainlen[duplicate]) &&
          ((i - rankstart) > bestlen)) {
        beststart = rankstart;
        bestlen = i - rankstart;
      }

      rankstart = 0;
    }
  }

  if (bestlen > 0) {
    /* from the top of the chain down */
    for (i = beststart + bestlen - 1; i >= beststart; i--)
      CardArray_TakeRank(&hand->cards, &ctx->rcards, (uint8_t)i, duplicate);

    hand->type = Hand_Format(primal[duplicate], HAND_KICKER_NONE, HAND_CHAIN);
  }
}

typedef void (*HandList_SearchPrimalFunc)(hand_ctx_t *, hand_t *, int);

#define HAND_SEARCH_TYPES 3

/*
 * pass a empty hand to start traverse
 * result stores in hand
 * return 0 when stop
 */
static int HLAA_TraverseChains(hand_ctx_t *ctx, int *begin, hand_t *hand) {
  int found = 0;
  int i = *begin;
  int primals[] = {1, 2, 3};

  /* solo chain, pair chain, trio chain, trio, pair, solo */
  HandList_SearchPrimalFunc searchers[HAND_SEARCH_TYPES];

  searchers[0] = HandList_SearchLongestConsecutive;
  searchers[1] = HandList_SearchLongestConsecutive;
  searchers[2] = HandList_SearchLongestConsecutive;

  if (CardArray_IsEmpty(&ctx->cards))
    return 0;

  if (*begin >= HAND_SEARCH_TYPES)
    return 0;

  /* init search */
  if (hand->type == 0) {
    while (i < HAND_SEARCH_TYPES && hand->type == 0) {
      searchers[i](ctx, hand, primals[i]);

      if (hand->type != 0) {
        found = 1;
        break;
      } else {
        i++;
        *begin = i;
      }
    }

    /* if found == 0, should PANIC */
  } else {
    /* continue search via beat */
    found = Beat_Search(&ctx->cards, hand, hand);
  }

  return found;
}

/*
 * extract all chains or primal hands in hand_ctx
 */
static void HLAA_ExtractAllChains(hand_ctx_t *ctx, hand_list_t *hands) {
  int found = 0;
  int lastsearch = 0;
  hand_t workinghand;
  hand_t lasthand;

  /* init search */
  Hand_Clear(&workinghand);
  Hand_Clear(&lasthand);

  found = HLAA_TraverseChains(ctx, &lastsearch, &lasthand);

  while (found != 0) {
    HandList_Push(hands, &lasthand);

    Hand_Copy(&workinghand, &lasthand);

    while ((found = HLAA_TraverseChains(ctx, &lastsearch, &workinghand)) != 0)
      HandList_Push(hands, &workinghand);

    /* can't find any more hands, try to reduce chain length */
    if (lasthand.type != 0) {
      if (lasthand.type ==
          Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAIN)) {
        if (CardArray_Length(&lasthand.cards) > HAND_SOLO_CHAIN_MIN_LENGTH) {
          CardArray_DropFront(&lasthand.cards, 1);
          found = 1;
        } else {
          lasthand.type = 0;
        }
      } else if (
          lasthand.type ==
          Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAIN)) {
        if (CardArray_Length(&lasthand.cards) > HAND_PAIR_CHAIN_MIN_LENGTH) {
          CardArray_DropFront(&lasthand.cards, 2);
          found = 1;
        } else {
          lasthand.type = 0;
        }
      } else if (
          lasthand.type ==
          Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAIN)) {
        if (CardArray_Length(&lasthand.cards) > HAND_TRIO_CHAIN_MIN_LENGTH) {
          CardArray_DropFront(&lasthand.cards, 3);
          found = 1;
        } else {
          lasthand.type = 0;
        }
      }

      /* still can't found, loop through hand type for more */
      if (found == 0) {
        lastsearch++;
        Hand_Clear(&lasthand);
        found = HLAA_TraverseChains(ctx, &lastsearch, &lasthand);
      }
    }
  }
}

/*
 * Pulling a chain out leaves cards that may hold further chains, so the ways
 * to take cards apart form a tree: every node is the cards left after the
 * chains on the path to it. Only the path being walked is kept.
 */

/* every step takes at least five cards out of twenty */
#define ANALYSIS_MAX_DEPTH 4

typedef struct analysis_best_s {
  /* hands needed: chains on the path plus the leftover taken apart */
  int weight;
  /* chains pulled out, from the first to the last */
  int depth;
  hand_t path[ANALYSIS_MAX_DEPTH];
  /* what is left after them */
  card_array_t leftover;

} analysis_best_t;

static void
HLAA_Search(hand_ctx_t *ctx, hand_t *path, int depth, analysis_best_t *best) {
  hand_list_t chains;
  int i = 0;

  HandList_Clear(&chains);

  if (depth < ANALYSIS_MAX_DEPTH)
    HLAA_ExtractAllChains(ctx, &chains);

  if (HandList_Count(&chains) == 0) {
    /* nothing more to pull out, the rest is played as it is */
    int weight = depth + Analysis_CountHands(Analysis_Standard, &ctx->cards);

    /* on a tie the split found last wins */
    if (weight <= best->weight) {
      best->weight = weight;
      best->depth = depth;
      memcpy(best->path, path, sizeof(hand_t) * (size_t)depth);
      CardArray_Copy(&best->leftover, &ctx->cards);
    }

    return;
  }

  for (i = 0; i < HandList_Count(&chains); i++) {
    hand_ctx_t rest;

    /* the cards without this chain */
    Hand_Copy(&path[depth], HandList_At(&chains, i));
    CardArray_Copy(&rest.cards, &ctx->cards);
    CardArray_Subtract(&rest.cards, &path[depth].cards);
    CardArray_Copy(&rest.rcards, &rest.cards);
    CardArray_Reverse(&rest.rcards);
    CardArray_CountRanks(&rest.cards, rest.count);

    HLAA_Search(&rest, path, depth + 1, best);
  }
}

/*
 * search hand via least hands
 */
void Analysis_Advanced(const card_array_t *array, hand_list_t *hl) {
  hand_list_t bombs;
  hand_t path[ANALYSIS_MAX_DEPTH];
  analysis_best_t best;
  hand_ctx_t ctx;
  int i = 0;

  HandCtx_Clear(&ctx);
  CardArray_CountRanks(array, ctx.count);
  CardArray_Copy(&ctx.cards, array);
  CardArray_Sort(&ctx.cards);

  /* nuke, bombs and 2 are never broken up */
  HandList_Clear(&bombs);
  HandList_ExtractNukeBomb2(&bombs, &ctx.cards, ctx.count);

  CardArray_Copy(&ctx.rcards, &ctx.cards);
  CardArray_Reverse(&ctx.rcards);

  best.weight = INT_MAX;
  best.depth = 0;
  CardArray_Clear(&best.leftover);
  HLAA_Search(&ctx, path, 0, &best);

  /* no chains at all, this is the standard analysis */
  if (best.depth == 0) {
    Analysis_Standard(array, hl);
    return;
  }

  /* the leftover, then the chains from the last pulled to the first */
  Analysis_Standard(&best.leftover, hl);

  for (i = best.depth - 1; i >= 0; i--)
    HandList_Push(hl, &best.path[i]);

  for (i = 0; i < HandList_Count(&bombs); i++)
    HandList_Push(hl, HandList_At(&bombs, i));
}
