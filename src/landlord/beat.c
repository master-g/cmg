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

#include "beat.h"
#include "lmath.h"

/* beat search context */
typedef struct beat_ctx_s {
  /* rank count */
  int count[CARD_RANK_END];
  /* the cards, sorted from high to low */
  card_array_t cards;
  /* the cards, sorted from low to high */
  card_array_t rcards;

} beat_ctx_t;

static void beat_ctx_setup(beat_ctx_t *ctx, const card_array_t *array) {
  memset(ctx, 0, sizeof(beat_ctx_t));

  card_array_count_ranks(array, ctx->count);
  card_array_copy(&ctx->cards, array);
  card_array_sort(&ctx->cards);
  card_array_copy(&ctx->rcards, &ctx->cards);
  card_array_reverse(&ctx->rcards);
}

/* how many ranks the primal part of a hand spans */
static int beat_primal_ranks(const card_array_t *primal) {
  int count[CARD_RANK_END];
  int rank = 0;
  int ranks = 0;

  card_array_count_ranks(primal, count);
  for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
    if (count[rank] != 0)
      ranks++;
  }

  return ranks;
}

/* the lowest rank in some cards, CARD_RANK_END when there is none */
static int beat_lowest_rank(const card_array_t *array) {
  int count[CARD_RANK_END];
  int rank = 0;

  card_array_count_ranks(array, count);
  for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
    if (count[rank] != 0)
      break;
  }

  return rank;
}

/* the lowest rank above `above` held at least `primal` times */
static int beat_search_primal(
    const beat_ctx_t *ctx, const card_array_t *tobeat, hand_type_t tobeattype,
    hand_t *beat, int primal) {
  int rank = 0;
  int above = CARD_RANK(card_array_at(tobeat, 0));

  for (rank = above + 1; rank < CARD_RANK_END; rank++) {
    if (ctx->count[rank] >= primal) {
      hand_clear(beat);
      beat->type = tobeattype;
      card_array_take_rank(&beat->cards, &ctx->rcards, rank, primal);
      return 1;
    }
  }

  return 0;
}

static int
beat_search_bomb(const beat_ctx_t *ctx, const hand_t *tobeat, hand_t *beat) {
  int canbeat = 0;
  int rank = 0;

  /*
   * This only decides where to look, beat_search_all asks the rules
   * whether what was found really beats the hand.
   */
  if (hand_is_nuke(tobeat))
    return 0;

  /* search for a higher rank bomb */
  if (hand_is_bomb(tobeat)) {
    canbeat = beat_search_primal(ctx, &tobeat->cards, tobeat->type, beat, 4);
  } else {
    /* tobeat is not a nuke or bomb, search a bomb to beat it */
    for (rank = CARD_RANK_END - 1; rank >= CARD_RANK_BEG; rank--) {
      if (ctx->count[rank] == 4) {
        canbeat = 1;
        hand_clear(beat);
        card_array_copy_rank(&beat->cards, &ctx->cards, rank);
        break;
      }
    }
  }

  /* search for nuke */
  if (canbeat == 0) {
    if (ctx->count[CARD_RANK_BLACK_JOKER] && ctx->count[CARD_RANK_RED_JOKER]) {
      canbeat = 1;
      hand_clear(beat);
      beat->type = hand_type(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, false);
      card_array_copy_rank(&beat->cards, &ctx->cards, CARD_RANK_RED_JOKER);
      card_array_copy_rank(&beat->cards, &ctx->cards, CARD_RANK_BLACK_JOKER);
    }
  } else {
    beat->type = hand_type(HAND_PRIMAL_BOMB, HAND_KICKER_NONE, false);
  }

  return canbeat;
}

/*
 * for a standard 54 card set, each rank has four cards
 * so it is impossible for two trio of the same rank at the same time
 *
 * a) player_1 SEARCH_BEAT player_2 : impossible for 333 vs 333
 *
 * BUT
 *
 * b) player_1 SEARCH_BEAT_LOOP player_1_prev_beat : possible for 333 vs 333
 *
 */
static int beat_search_trio_kicker(
    const beat_ctx_t *ctx, const hand_t *tobeat, hand_t *beat, int kick) {
  int rank = 0;
  int canbeat = 0;
  int triorank = 0;
  int kickrank = 0;
  card_array_t trio, kicker;
  hand_t htriobeat, hkickbeat;
  /* beat may be tobeat itself, so take what is needed from it first */
  const hand_type_t tobeattype = tobeat->type;

  hand_clear(&htriobeat);
  hand_clear(&hkickbeat);
  hand_split(tobeat, &trio, &kicker);
  triorank = CARD_RANK(card_array_at(&trio, 0));
  kickrank = CARD_RANK(card_array_at(&kicker, 0));

  /* same rank trio , case b: keep the trio and search for a higher kicker */
  if (card_array_contains(&ctx->rcards, &trio)) {
    for (rank = kickrank + 1; rank < CARD_RANK_END; rank++) {
      if ((rank != triorank) && (ctx->count[rank] >= kick)) {
        card_array_copy(&htriobeat.cards, &trio);
        card_array_take_rank(&hkickbeat.cards, &ctx->rcards, rank, kick);
        canbeat = 1;
        break;
      }
    }
  }

  /*
   * same rank trio not found
   * OR
   * same rank trio found, but kicker can't beat
   */
  if (canbeat == 0) {
    /* trio beat found, search for the lowest kicker */
    if (beat_search_primal(
            ctx, &trio, hand_type(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, false),
            &htriobeat, HAND_PRIMAL_TRIO)) {
      triorank = hand_rank(&htriobeat);

      for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
        if ((rank != triorank) && (ctx->count[rank] >= kick)) {
          card_array_take_rank(&hkickbeat.cards, &ctx->rcards, rank, kick);
          canbeat = 1;
          break;
        }
      }
    }
  }

  /* beat */
  if (canbeat == 1) {
    hand_clear(beat);
    card_array_concat(&beat->cards, &htriobeat.cards);
    card_array_concat(&beat->cards, &hkickbeat.cards);
    beat->type = tobeattype;
  }

  return canbeat;
}

/*
 * the lowest chain of the same length that starts above the chain in
 * `tobeat`, every rank held at least `duplicate` times
 */
static int beat_search_chain(
    const beat_ctx_t *ctx, const card_array_t *tobeat, hand_type_t tobeattype,
    hand_t *beat, int duplicate) {
  int i, j;
  int chainlength = card_array_length(tobeat) / duplicate;
  int footer = beat_lowest_rank(tobeat);

  /* search for beat chain in rank counts */
  for (i = footer + 1; i <= CARD_RANK_2 - chainlength; i++) {
    int found = 1;

    for (j = 0; j < chainlength; j++) {
      /* check if chain breaks */
      if (ctx->count[i + j] < duplicate) {
        found = 0;
        break;
      }
    }

    if (found) {
      hand_clear(beat);
      beat->type = tobeattype;

      /* from the top of the chain down, the low suits of every rank */
      for (j = chainlength - 1; j >= 0; j--) {
        card_array_t rank;

        card_array_clear(&rank);
        card_array_take_rank(&rank, &ctx->rcards, (i + j), duplicate);
        card_array_reverse(&rank);
        card_array_concat(&beat->cards, &rank);
      }

      return 1;
    }
  }

  return 0;
}

static int beat_search_trio_kicker_chain(
    const beat_ctx_t *ctx, const hand_t *tobeat, hand_t *beat, int kc) {
  int canbeat = 0;
  int i, j, rank, chainlength;
  int triocount[CARD_RANK_END];
  int kickcount[CARD_RANK_END];
  int combrankmap[CARD_RANK_END];
  int rankcombmap[CARD_RANK_END];
  int comb[CARD_RANK_END];
  card_array_t trio, kicker;
  hand_t htriobeat, hkickbeat;
  /* beat may be tobeat itself, so take what is needed from it first */
  const hand_type_t tobeattype = tobeat->type;

  hand_clear(&htriobeat);
  hand_clear(&hkickbeat);
  hand_split(tobeat, &trio, &kicker);
  chainlength = beat_primal_ranks(&trio);

  /* self beat, see beat_search_trio_kicker */
  if (card_array_contains(&ctx->rcards, &trio)) {
    int n = 0; /* combination total */

    /* the ranks that can be kickers: not a trio of the chain, enough cards */
    card_array_count_ranks(&trio, triocount);
    memset(combrankmap, -1, sizeof(int) * CARD_RANK_END);
    memset(rankcombmap, -1, sizeof(int) * CARD_RANK_END);

    for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
      if ((triocount[rank] == 0) && (ctx->count[rank] >= kc)) {
        combrankmap[n] = rank;
        rankcombmap[rank] = n;
        n++;
      }
    }

    /* the kickers of tobeat as a combination of those ranks, ascending */
    j = 0;
    memset(comb, -1, sizeof(int) * CARD_RANK_END);
    card_array_count_ranks(&kicker, kickcount);

    for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
      if (kickcount[rank] != 0)
        comb[j++] = rankcombmap[rank];
    }

    /* find next combination */
    if (lmath_next_comb(comb, chainlength, n)) {
      /* next combination found, copy kickers */
      for (i = 0; i < chainlength; i++)
        card_array_take_rank(
            &hkickbeat.cards, &ctx->rcards, combrankmap[comb[i]], kc);

      canbeat = 1;

      /* copy trio to beat */
      card_array_concat(&htriobeat.cards, &trio);
      card_array_sort(&hkickbeat.cards);
    }
  }

  /* can't find same rank trio chain, search for higher rank trio */
  if (canbeat == 0) {
    /* higher rank trio chain found, search for the lowest kickers */
    if (beat_search_chain(
            ctx, &trio, hand_type(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, true),
            &htriobeat, HAND_PRIMAL_TRIO)) {
      int kickers = 0;

      card_array_count_ranks(&htriobeat.cards, triocount);

      for (rank = CARD_RANK_BEG;
           (rank < CARD_RANK_END) && (kickers < chainlength); rank++) {
        if ((triocount[rank] == 0) && (ctx->count[rank] >= kc)) {
          card_array_take_rank(&hkickbeat.cards, &ctx->rcards, rank, kc);
          kickers++;
        }
      }

      if (kickers == chainlength)
        canbeat = 1;
    }
  }

  /* final */
  if (canbeat) {
    hand_clear(beat);
    card_array_concat(&beat->cards, &htriobeat.cards);
    card_array_concat(&beat->cards, &hkickbeat.cards);
    beat->type = tobeattype;
  }

  return canbeat;
}

static int
beat_search_any(const card_array_t *cards, const hand_t *tobeat, hand_t *beat) {
  int canbeat = 0;
  beat_ctx_t ctx;

  /* setup search context */
  beat_ctx_setup(&ctx, cards);

  /* start search: the same kind of hand, only higher */
  switch (tobeat->type.primal) {
  case HAND_PRIMAL_SOLO:
  case HAND_PRIMAL_PAIR:
    /* never carry kickers */
    if (tobeat->type.chain)
      canbeat = beat_search_chain(
          &ctx, &tobeat->cards, tobeat->type, beat, (int)tobeat->type.primal);
    else
      canbeat = beat_search_primal(
          &ctx, &tobeat->cards, tobeat->type, beat, (int)tobeat->type.primal);
    break;

  case HAND_PRIMAL_TRIO:
    switch (tobeat->type.kicker) {
    case HAND_KICKER_NONE:
      if (tobeat->type.chain)
        canbeat = beat_search_chain(
            &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_TRIO);
      else
        canbeat = beat_search_primal(
            &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_TRIO);
      break;

    case HAND_KICKER_SOLO:
    case HAND_KICKER_PAIR: {
      /* cards per kicker */
      int kick = tobeat->type.kicker == HAND_KICKER_SOLO ? 1 : 2;

      if (tobeat->type.chain)
        canbeat = beat_search_trio_kicker_chain(&ctx, tobeat, beat, kick);
      else
        canbeat = beat_search_trio_kicker(&ctx, tobeat, beat, kick);
      break;
    }

    case HAND_KICKER_DUAL_SOLO:
    case HAND_KICKER_DUAL_PAIR:
      /* a trio never carries two kickers */
      break;
    }
    break;

  case HAND_PRIMAL_FOUR:
    /* only a bare chain of fours is searched, four with kickers is not */
    if (tobeat->type.chain && (tobeat->type.kicker == HAND_KICKER_NONE))
      canbeat = beat_search_chain(
          &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_FOUR);
    break;

  case HAND_PRIMAL_NONE:
  case HAND_PRIMAL_BOMB:
  case HAND_PRIMAL_NUKE:
    /* only a bomb or the nuke can answer, see below */
    break;
  }

  /* search for bomb/nuke */
  if (canbeat == 0)
    canbeat = beat_search_bomb(&ctx, tobeat, beat);

  return canbeat;
}

int beat_search(const card_array_t *cards, const hand_t *tobeat, hand_t *beat) {
  /* already in search loop, continue */
  if (!hand_is_none(beat))
    return beat_search_any(cards, beat, beat);
  else
    return beat_search_any(cards, tobeat, beat);
}

void beat_search_all(
    const card_array_t *cards, const hand_t *tobeat, hand_list_t *hl) {
  hand_t htobeat;
  hand_t beat;
  hand_t judged;

  hand_list_clear(hl);
  hand_clear(&beat);
  hand_copy(&htobeat, tobeat);

  while (beat_search_any(cards, &htobeat, &beat)) {
    /* keep searching from this one, whatever the rules say about it */
    hand_copy(&htobeat, &beat);

    /* the search proposes, the rules decide */
    if (hand_parse(&judged, &beat.cards) &&
        (hand_compare(&judged, tobeat) == HAND_CMP_GREATER)) {
      if (!hand_list_push(hl, &beat))
        break;
    }
  }
}
