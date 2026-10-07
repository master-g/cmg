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

  if ((duplicate < HAND_PRIMAL_SOLO) || (duplicate > HAND_PRIMAL_TRIO))
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
    if (count[i] == HAND_PRIMAL_FOUR) {
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

    /* one to three of them, four would have been a bomb */
    hand.type =
        hand_type((hand_primal_t)count[CARD_RANK_2], HAND_KICKER_NONE, false);
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
  analysis_extract_consecutive(hl, &array, count, HAND_PRIMAL_TRIO);
  analysis_extract_consecutive(hl, &array, count, HAND_PRIMAL_PAIR);
  analysis_extract_consecutive(hl, &array, count, HAND_PRIMAL_SOLO);
}

/*
 * ************************************************************
 * counting turns
 * ************************************************************
 */

/* a chain pulled out: `ranks` ranks from `low` up, `primal` cards of each */
typedef struct analysis_chain_s {
  int primal;
  int low;
  int ranks;
} analysis_chain_t;

/* every chain takes at least five cards out of twenty */
#define ANALYSIS_MAX_DEPTH 4

typedef struct analysis_counted_s {
  int best; /* fewest turns so far */
  int depth;
  analysis_chain_t chains[ANALYSIS_MAX_DEPTH]; /* the chains that gave it */
  analysis_chain_t path[ANALYSIS_MAX_DEPTH];   /* the split being tried */
} analysis_counted_t;

/*
 * turns it takes to play the chains pulled out and what is left of the
 * counts: every trio takes a solo or a pair along, a trio chain takes one for
 * each of its ranks, all solos or all pairs
 */
static int analysis_counted_turns(
    const int *count, const analysis_chain_t *path, int depth) {
  int n[HAND_PRIMAL_FOUR + 1] = {0}; /* n[k]: ranks held k times */
  int rank = 0;
  int turns = depth;
  int i = 0;

  for (rank = CARD_RANK_3; rank <= CARD_RANK_A; rank++)
    n[count[rank]]++;

  turns += n[HAND_PRIMAL_SOLO] + n[HAND_PRIMAL_PAIR] + n[HAND_PRIMAL_TRIO];

  for (i = 0; i < depth; i++) {
    if (path[i].primal != HAND_PRIMAL_TRIO)
      continue;

    if (n[HAND_PRIMAL_SOLO] >= path[i].ranks) {
      n[HAND_PRIMAL_SOLO] -= path[i].ranks;
      turns -= path[i].ranks;
    } else if (n[HAND_PRIMAL_PAIR] >= path[i].ranks) {
      n[HAND_PRIMAL_PAIR] -= path[i].ranks;
      turns -= path[i].ranks;
    }
  }

  /* a trio with a kicker is one turn, so every kicker taken saves one */
  i = n[HAND_PRIMAL_SOLO] + n[HAND_PRIMAL_PAIR];
  turns -= n[HAND_PRIMAL_TRIO] < i ? n[HAND_PRIMAL_TRIO] : i;

  return turns;
}

static void
analysis_counted_search(int *count, analysis_counted_t *state, int depth) {
  /* ranks a chain needs at least */
  static const int minranks[] = {
      0, HAND_SOLO_CHAIN_MIN_LENGTH, HAND_PAIR_CHAIN_MIN_LENGTH / 2,
      HAND_TRIO_CHAIN_MIN_LENGTH / 3};
  int turns = analysis_counted_turns(count, state->path, depth);
  int primal = 0;
  int low = 0;
  int top = 0;
  int rank = 0;

  /* on a tie the split found first wins */
  if (turns < state->best) {
    state->best = turns;
    state->depth = depth;
    memcpy(state->chains, state->path, sizeof(state->path));
  }

  /* another chain is another turn at least */
  if ((depth == ANALYSIS_MAX_DEPTH) || (depth + 1 >= state->best))
    return;

  for (primal = HAND_PRIMAL_SOLO; primal <= HAND_PRIMAL_TRIO; primal++) {
    for (low = CARD_RANK_3; low <= CARD_RANK_A; low++) {
      /* grow the chain one rank at a time, 2 and jokers never chain */
      for (top = low; (top <= CARD_RANK_A) && (count[top] >= primal); top++) {
        count[top] -= primal;

        if (top - low + 1 >= minranks[primal]) {
          state->path[depth].primal = primal;
          state->path[depth].low = low;
          state->path[depth].ranks = top - low + 1;
          analysis_counted_search(count, state, depth + 1);
        }
      }

      for (rank = low; rank < top; rank++)
        count[rank] += primal;
    }
  }
}

/*
 * nuke, bombs and 2 go to `bombs` and are never broken up, each is a turn of
 * its own; `rest` is what the search worked on, sorted, and state holds the
 * chains to pull out of it
 */
static void analysis_counted_split(
    const card_array_t *array, card_array_t *rest, hand_list_t *bombs,
    analysis_counted_t *state) {
  int count[CARD_RANK_END];

  card_array_copy(rest, array);
  card_array_sort(rest);
  card_array_count_ranks(rest, count);

  hand_list_clear(bombs);
  analysis_extract_nuke_bomb_2(bombs, rest, count);

  memset(state, 0, sizeof(*state));
  state->best = INT_MAX;
  analysis_counted_search(count, state, 0);
}

int analysis_counted_hands(const card_array_t *array) {
  card_array_t rest;
  hand_list_t bombs;
  analysis_counted_t state;

  analysis_counted_split(array, &rest, &bombs, &state);

  return hand_list_count(&bombs) + state.best;
}

void analysis_counted(const card_array_t *array, hand_list_t *hl) {
  card_array_t rest;
  hand_list_t bombs;
  analysis_counted_t state;
  hand_t chains[ANALYSIS_MAX_DEPTH];
  int i = 0;
  int rank = 0;

  analysis_counted_split(array, &rest, &bombs, &state);

  /* the counts chose the chains, now take the cards */
  for (i = 0; i < state.depth; i++) {
    const analysis_chain_t *chain = &state.chains[i];

    hand_clear(&chains[i]);
    chains[i].type =
        hand_type((hand_primal_t)chain->primal, HAND_KICKER_NONE, true);

    for (rank = chain->low + chain->ranks - 1; rank >= chain->low; rank--)
      card_array_take_rank(&chains[i].cards, &rest, rank, chain->primal);

    card_array_subtract(&rest, &chains[i].cards);
  }

  /* the leftover, then the chains from the last pulled to the first */
  analysis_standard(&rest, hl);

  for (i = state.depth - 1; i >= 0; i--)
    hand_list_push(hl, &chains[i]);

  for (i = 0; i < hand_list_count(&bombs); i++)
    hand_list_push(hl, hand_list_at(&bombs, i));
}
