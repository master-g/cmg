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

#include "move.h"

#include <stddef.h>

/* the most kickers a hand carries: one for each trio of the longest chain */
#define MOVE_MAX_KICKERS (HAND_MAX_LENGTH / (HAND_PRIMAL_TRIO + 1))

/* what is being generated: only the moves that beat tobeat when there is one */
typedef struct move_ctx_s {
  const card_array_t *cards;
  const hand_t *tobeat;
  hand_list_t *moves;
} move_ctx_t;

/*
 * `per` cards of each of `ranks` ranks from `low` up, and `kickper` cards of
 * each kicker rank; the rules decide what it is and whether it is a hand
 */
static void move_push(
    const move_ctx_t *ctx, int low, int ranks, int per, const int *kickers,
    int nkickers, int kickper) {
  const card_array_t *cards = ctx->cards;
  card_array_t picked;
  hand_t hand;
  int i = 0;

  card_array_clear(&picked);

  for (i = 0; i < ranks; i++)
    card_array_take_rank(&picked, cards, low + i, per);

  for (i = 0; i < nkickers; i++)
    card_array_take_rank(&picked, cards, kickers[i], kickper);

  if (!hand_parse(&hand, &picked))
    return;

  if ((ctx->tobeat == NULL) ||
      (hand_compare(&hand, ctx->tobeat) == HAND_CMP_GREATER))
    hand_list_push(ctx->moves, &hand);
}

/*
 * the body carrying `want` kickers of `kickper` cards each
 * ponytail: one choice of kickers, the ranks held the fewest times and the
 * lowest of those; enumerate the combinations if the AI needs the others
 */
static void move_push_with_kickers(
    const move_ctx_t *ctx, const int *count, int low, int ranks, int per,
    int want, int kickper) {
  int kickers[MOVE_MAX_KICKERS];
  int n = 0;
  int held = 0;
  int rank = 0;

  /* more cards than one seat ever holds, when asked about several seats */
  if (want > MOVE_MAX_KICKERS)
    return;

  for (held = kickper; (held <= HAND_PRIMAL_FOUR) && (n < want); held++) {
    for (rank = CARD_RANK_BEG; (rank < CARD_RANK_END) && (n < want); rank++) {
      if ((count[rank] == held) && ((rank < low) || (rank >= low + ranks)))
        kickers[n++] = rank;
    }
  }

  if (n == want)
    move_push(ctx, low, ranks, per, kickers, n, kickper);
}

static void move_generate_all(const move_ctx_t *ctx) {
  const card_array_t *cards = ctx->cards;
  /* ranks a chain needs at least */
  static const int minranks[] = {
      0, HAND_SOLO_CHAIN_MIN_LENGTH, HAND_PAIR_CHAIN_MIN_LENGTH / 2,
      HAND_TRIO_CHAIN_MIN_LENGTH / 3};
  int count[CARD_RANK_END];
  int rank = 0;
  int per = 0;
  int top = 0;
  int kicker = 0;

  hand_list_clear(ctx->moves);
  card_array_count_ranks(cards, count);

  /* solo, pair, trio and bomb of every rank */
  for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
    for (per = HAND_PRIMAL_SOLO; per <= count[rank]; per++)
      move_push(ctx, rank, 1, per, NULL, 0, 0);
  }

  /* nuke */
  if (count[CARD_RANK_BLACK_JOKER] && count[CARD_RANK_RED_JOKER])
    move_push(ctx, CARD_RANK_BLACK_JOKER, 2, 1, NULL, 0, 0);

  /* chains, and trio chains with their kickers */
  for (per = HAND_PRIMAL_SOLO; per <= HAND_PRIMAL_TRIO; per++) {
    for (rank = CARD_RANK_3; rank <= CARD_RANK_A; rank++) {
      for (top = rank; (top <= CARD_RANK_A) && (count[top] >= per); top++) {
        int ranks = top - rank + 1;

        if (ranks < minranks[per])
          continue;

        move_push(ctx, rank, ranks, per, NULL, 0, 0);

        if (per == HAND_PRIMAL_TRIO) {
          move_push_with_kickers(
              ctx, count, rank, ranks, per, ranks, HAND_PRIMAL_SOLO);
          move_push_with_kickers(
              ctx, count, rank, ranks, per, ranks, HAND_PRIMAL_PAIR);
        }
      }
    }
  }

  for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
    /* a trio with every kicker it could carry */
    if (count[rank] >= HAND_PRIMAL_TRIO) {
      for (kicker = CARD_RANK_BEG; kicker < CARD_RANK_END; kicker++) {
        if (kicker == rank)
          continue;

        for (per = HAND_PRIMAL_SOLO;
             (per <= HAND_PRIMAL_PAIR) && (per <= count[kicker]); per++)
          move_push(ctx, rank, 1, HAND_PRIMAL_TRIO, &kicker, 1, per);
      }
    }

    /* four with two solos, four with two pairs */
    if (count[rank] == HAND_PRIMAL_FOUR) {
      move_push_with_kickers(
          ctx, count, rank, 1, HAND_PRIMAL_FOUR, 2, HAND_PRIMAL_SOLO);
      move_push_with_kickers(
          ctx, count, rank, 1, HAND_PRIMAL_FOUR, 2, HAND_PRIMAL_PAIR);
    }
  }
}

void move_generate(const card_array_t *cards, hand_list_t *moves) {
  move_ctx_t ctx;

  ctx.cards = cards;
  ctx.tobeat = NULL;
  ctx.moves = moves;
  move_generate_all(&ctx);
}

void move_generate_beats(
    const card_array_t *cards, const hand_t *tobeat, hand_list_t *moves) {
  move_ctx_t ctx;

  ctx.cards = cards;
  ctx.tobeat = tobeat;
  ctx.moves = moves;
  move_generate_all(&ctx);
}
