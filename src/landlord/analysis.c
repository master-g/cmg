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
#include <string.h>

/*
 * extract hands like 34567 / 334455 / 333444555 etc
 * from the ranks held exactly `duplicate` times: consecutive ranks become one
 * chain when there are enough of them, single hands otherwise
 * cards has to be sorted from high to low
 */
static void analysis_extract_consecutive(
    hand_list_t *hl, const card_array_t *cards, const int *count,
    int duplicate) {
  int rank = 0;
  int runtop = 0; /* highest rank of the run being collected */
  int runlen = 0; /* ranks in that run */
  hand_t hand;
  hand_primal_t primal[] = {
      HAND_PRIMAL_NONE, HAND_PRIMAL_SOLO, HAND_PRIMAL_PAIR, HAND_PRIMAL_TRIO};
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
      hand_clear(&hand);
      hand.type = hand_type(primal[duplicate], HAND_KICKER_NONE, true);

      for (r = runtop; r > runtop - runlen; r--)
        card_array_copy_rank(&hand.cards, cards, r);

      hand_list_push(hl, &hand);
    } else {
      /* not a chain */
      for (r = runtop; r > runtop - runlen; r--) {
        hand_clear(&hand);
        hand.type = hand_type(primal[duplicate], HAND_KICKER_NONE, false);
        card_array_copy_rank(&hand.cards, cards, r);
        hand_list_push(hl, &hand);
      }
    }

    runlen = 0;
  }
}

/* extract nuke/bomb/2 from array, these cards will be removed from array */
static void
analysis_extract_nuke_bomb_2(hand_list_t *hl, card_array_t *array, int *count) {
  int i = 0;
  hand_t hand;

  /* nuke */
  if (count[CARD_RANK_BLACK_JOKER] && count[CARD_RANK_RED_JOKER]) {
    hand_clear(&hand);
    hand.type = hand_type(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, false);
    card_array_copy_rank(&hand.cards, array, CARD_RANK_RED_JOKER);
    card_array_copy_rank(&hand.cards, array, CARD_RANK_BLACK_JOKER);

    hand_list_push(hl, &hand);

    count[CARD_RANK_BLACK_JOKER] = 0;
    count[CARD_RANK_RED_JOKER] = 0;

    card_array_remove_rank(array, CARD_RANK_BLACK_JOKER);
    card_array_remove_rank(array, CARD_RANK_RED_JOKER);
  }

  /* bomb */
  for (i = CARD_RANK_2; i >= CARD_RANK_3; i--) {
    if (count[i] == 4) {
      hand_clear(&hand);
      hand.type = hand_type(HAND_PRIMAL_BOMB, HAND_KICKER_NONE, false);
      card_array_copy_rank(&hand.cards, array, i);

      hand_list_push(hl, &hand);

      count[i] = 0;
      card_array_remove_rank(array, i);
    }
  }

  /* joker */
  if ((count[CARD_RANK_BLACK_JOKER] != 0) ||
      (count[CARD_RANK_RED_JOKER] != 0)) {
    hand_clear(&hand);
    card_array_copy_rank(
        &hand.cards, array,
        count[CARD_RANK_BLACK_JOKER] != 0 ? CARD_RANK_BLACK_JOKER
                                          : CARD_RANK_RED_JOKER);
    hand.type = hand_type(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, false);

    hand_list_push(hl, &hand);
    count[CARD_RANK_BLACK_JOKER] = 0;
    count[CARD_RANK_RED_JOKER] = 0;
    card_array_remove_rank(array, CARD_RANK_BLACK_JOKER);
    card_array_remove_rank(array, CARD_RANK_RED_JOKER);
  }

  /* 2 */
  if (count[CARD_RANK_2] != 0) {
    hand_clear(&hand);
    card_array_copy_rank(&hand.cards, array, CARD_RANK_2);

    switch (count[CARD_RANK_2]) {
    case 1:
      hand.type = hand_type(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, false);
      break;

    case 2:
      hand.type = hand_type(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, false);
      break;

    case 3:
      hand.type = hand_type(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, false);
      break;

    default:
      break;
    }
    count[CARD_RANK_2] = 0;
    card_array_remove_rank(array, CARD_RANK_2);
    hand_list_push(hl, &hand);
  }
}

void analysis_standard(const card_array_t *cards, hand_list_t *hl) {
  int count[CARD_RANK_END];
  card_array_t array;

  card_array_copy(&array, cards);
  card_array_sort(&array);
  card_array_count_ranks(&array, count);

  hand_list_clear(hl);

  /* nuke, bomb and 2 */
  analysis_extract_nuke_bomb_2(hl, &array, count);

  /* trios, pairs and solos, chained up where they can */
  analysis_extract_consecutive(hl, &array, count, 3);
  analysis_extract_consecutive(hl, &array, count, 2);
  analysis_extract_consecutive(hl, &array, count, 1);
}

int analysis_count_hands(analysis_func_t analyze, const card_array_t *array) {
  hand_list_t hl;

  analyze(array, &hl);

  return hand_list_count(&hl);
}

/*
 * ************************************************************
 * advanced analysis
 * ************************************************************
 */

/* cards being taken apart */
typedef struct analysis_ctx_s {
  /* rank count */
  int count[CARD_RANK_END];
  /* original cards */
  card_array_t cards;
  /* the cards, low to high */
  card_array_t rcards;

} analysis_ctx_t;

/* the longest chain of ranks held at least `duplicate` times, lowest wins */
static void analysis_search_longest_chain(
    const analysis_ctx_t *ctx, hand_t *hand, int duplicate) {
  int i = 0;
  int rankstart = 0;
  int beststart = 0;
  int bestlen = 0;
  hand_primal_t primal[] = {
      HAND_PRIMAL_NONE, HAND_PRIMAL_SOLO, HAND_PRIMAL_PAIR, HAND_PRIMAL_TRIO};
  int chainlen[] = {
      0, HAND_SOLO_CHAIN_MIN_LENGTH, HAND_PAIR_CHAIN_MIN_LENGTH,
      HAND_TRIO_CHAIN_MIN_LENGTH};
  const int *count = ctx->count;

  if ((duplicate < 1) || (duplicate > 3))
    return;

  /* early break */
  if (card_array_length(&ctx->rcards) < chainlen[duplicate])
    return;

  hand_clear(hand);

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
      card_array_take_rank(&hand->cards, &ctx->rcards, i, duplicate);

    hand->type = hand_type(primal[duplicate], HAND_KICKER_NONE, true);
  }
}

typedef void (*analysis_search_func_t)(const analysis_ctx_t *, hand_t *, int);

#define ANALYSIS_SEARCH_TYPES 3

/*
 * pass a empty hand to start traverse
 * result stores in hand
 * return 0 when stop
 */
static int
analysis_traverse_chains(const analysis_ctx_t *ctx, int *begin, hand_t *hand) {
  int found = 0;
  int i = *begin;
  int primals[] = {1, 2, 3};

  /* solo chain, pair chain, trio chain, trio, pair, solo */
  analysis_search_func_t searchers[ANALYSIS_SEARCH_TYPES];

  searchers[0] = analysis_search_longest_chain;
  searchers[1] = analysis_search_longest_chain;
  searchers[2] = analysis_search_longest_chain;

  if (card_array_is_empty(&ctx->cards))
    return 0;

  if (*begin >= ANALYSIS_SEARCH_TYPES)
    return 0;

  /* init search */
  if (hand_is_none(hand)) {
    while (i < ANALYSIS_SEARCH_TYPES && hand_is_none(hand)) {
      searchers[i](ctx, hand, primals[i]);

      if (!hand_is_none(hand)) {
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
    found = beat_search(&ctx->cards, hand, hand);
  }

  return found;
}

/*
 * extract all chains or primal hands in hand_ctx
 */
static void
analysis_extract_all_chains(const analysis_ctx_t *ctx, hand_list_t *hands) {
  int found = 0;
  int lastsearch = 0;
  hand_t workinghand;
  hand_t lasthand;

  /* init search */
  hand_clear(&workinghand);
  hand_clear(&lasthand);

  found = analysis_traverse_chains(ctx, &lastsearch, &lasthand);

  while (found != 0) {
    hand_list_push(hands, &lasthand);

    hand_copy(&workinghand, &lasthand);

    while ((found = analysis_traverse_chains(ctx, &lastsearch, &workinghand)) !=
           0)
      hand_list_push(hands, &workinghand);

    /* can't find any more hands, try to reduce chain length */
    if (!hand_is_none(&lasthand)) {
      if (hand_is_type(&lasthand, HAND_PRIMAL_SOLO, HAND_KICKER_NONE, true)) {
        if (card_array_length(&lasthand.cards) > HAND_SOLO_CHAIN_MIN_LENGTH) {
          card_array_drop_front(&lasthand.cards, 1);
          found = 1;
        } else {
          lasthand.type = hand_type(HAND_PRIMAL_NONE, HAND_KICKER_NONE, false);
        }
      } else if (
          hand_is_type(&lasthand, HAND_PRIMAL_PAIR, HAND_KICKER_NONE, true)) {
        if (card_array_length(&lasthand.cards) > HAND_PAIR_CHAIN_MIN_LENGTH) {
          card_array_drop_front(&lasthand.cards, 2);
          found = 1;
        } else {
          lasthand.type = hand_type(HAND_PRIMAL_NONE, HAND_KICKER_NONE, false);
        }
      } else if (
          hand_is_type(&lasthand, HAND_PRIMAL_TRIO, HAND_KICKER_NONE, true)) {
        if (card_array_length(&lasthand.cards) > HAND_TRIO_CHAIN_MIN_LENGTH) {
          card_array_drop_front(&lasthand.cards, 3);
          found = 1;
        } else {
          lasthand.type = hand_type(HAND_PRIMAL_NONE, HAND_KICKER_NONE, false);
        }
      }

      /* still can't found, loop through hand type for more */
      if (found == 0) {
        lastsearch++;
        hand_clear(&lasthand);
        found = analysis_traverse_chains(ctx, &lastsearch, &lasthand);
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

static void analysis_search(
    const analysis_ctx_t *ctx, hand_t *path, int depth, analysis_best_t *best) {
  hand_list_t chains;
  int i = 0;

  hand_list_clear(&chains);

  if (depth < ANALYSIS_MAX_DEPTH)
    analysis_extract_all_chains(ctx, &chains);

  if (hand_list_count(&chains) == 0) {
    /* nothing more to pull out, the rest is played as it is */
    int weight = depth + analysis_count_hands(analysis_standard, &ctx->cards);

    /* on a tie the split found last wins */
    if (weight <= best->weight) {
      best->weight = weight;
      best->depth = depth;
      memcpy(best->path, path, sizeof(hand_t) * (size_t)depth);
      card_array_copy(&best->leftover, &ctx->cards);
    }

    return;
  }

  for (i = 0; i < hand_list_count(&chains); i++) {
    analysis_ctx_t rest;

    /* the cards without this chain */
    hand_copy(&path[depth], hand_list_at(&chains, i));
    card_array_copy(&rest.cards, &ctx->cards);
    card_array_subtract(&rest.cards, &path[depth].cards);
    card_array_copy(&rest.rcards, &rest.cards);
    card_array_reverse(&rest.rcards);
    card_array_count_ranks(&rest.cards, rest.count);

    analysis_search(&rest, path, depth + 1, best);
  }
}

/*
 * search hand via least hands
 */
void analysis_advanced(const card_array_t *array, hand_list_t *hl) {
  hand_list_t bombs;
  hand_t path[ANALYSIS_MAX_DEPTH];
  analysis_best_t best;
  analysis_ctx_t ctx;
  int i = 0;

  memset(&ctx, 0, sizeof(ctx));
  card_array_count_ranks(array, ctx.count);
  card_array_copy(&ctx.cards, array);
  card_array_sort(&ctx.cards);

  /* nuke, bombs and 2 are never broken up */
  hand_list_clear(&bombs);
  analysis_extract_nuke_bomb_2(&bombs, &ctx.cards, ctx.count);

  card_array_copy(&ctx.rcards, &ctx.cards);
  card_array_reverse(&ctx.rcards);

  best.weight = INT_MAX;
  best.depth = 0;
  card_array_clear(&best.leftover);
  analysis_search(&ctx, path, 0, &best);

  /* no chains at all, this is the standard analysis */
  if (best.depth == 0) {
    analysis_standard(array, hl);
    return;
  }

  /* the leftover, then the chains from the last pulled to the first */
  analysis_standard(&best.leftover, hl);

  for (i = best.depth - 1; i >= 0; i--)
    hand_list_push(hl, &best.path[i]);

  for (i = 0; i < hand_list_count(&bombs); i++)
    hand_list_push(hl, hand_list_at(&bombs, i));
}
