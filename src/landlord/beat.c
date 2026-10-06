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
typedef struct hand_ctx_s {
  /* rank count */
  int count[CARD_RANK_END];
  /* the cards, sorted from high to low */
  card_array_t cards;
  /* the cards, sorted from low to high */
  card_array_t rcards;

} hand_ctx_t;

static void HandCtx_Setup(hand_ctx_t *ctx, const card_array_t *array) {
  memset(ctx, 0, sizeof(hand_ctx_t));

  CardArray_CountRanks(array, ctx->count);
  CardArray_Copy(&ctx->cards, array);
  CardArray_Sort(&ctx->cards);
  CardArray_Copy(&ctx->rcards, &ctx->cards);
  CardArray_Reverse(&ctx->rcards);
}

/* how many ranks the primal part of a hand spans */
static int Hand_PrimalRanks(const card_array_t *primal) {
  int count[CARD_RANK_END];
  int rank = 0;
  int ranks = 0;

  CardArray_CountRanks(primal, count);
  for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
    if (count[rank] != 0)
      ranks++;
  }

  return ranks;
}

/* the lowest rank in some cards, CARD_RANK_END when there is none */
static int CardArray_LowestRank(const card_array_t *array) {
  int count[CARD_RANK_END];
  int rank = 0;

  CardArray_CountRanks(array, count);
  for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
    if (count[rank] != 0)
      break;
  }

  return rank;
}

/* the lowest rank above `above` held at least `primal` times */
static int SearchBeat_Primal(
    hand_ctx_t *ctx, const card_array_t *tobeat, int tobeattype, hand_t *beat,
    int primal) {
  int rank = 0;
  int above = CARD_RANK(CardArray_At(tobeat, 0));

  for (rank = above + 1; rank < CARD_RANK_END; rank++) {
    if (ctx->count[rank] >= primal) {
      Hand_Clear(beat);
      beat->type = (uint8_t)tobeattype;
      CardArray_TakeRank(&beat->cards, &ctx->rcards, (uint8_t)rank, primal);
      return 1;
    }
  }

  return 0;
}

static int SearchBeat_Bomb(hand_ctx_t *ctx, hand_t *tobeat, hand_t *beat) {
  int canbeat = 0;
  int rank = 0;

  /*
   * This only decides where to look, Beat_SearchAll asks the rules
   * whether what was found really beats the hand.
   */
  if (Hand_IsNuke(tobeat))
    return 0;

  /* search for a higher rank bomb */
  if (Hand_IsBomb(tobeat)) {
    canbeat = SearchBeat_Primal(ctx, &tobeat->cards, tobeat->type, beat, 4);
  } else {
    /* tobeat is not a nuke or bomb, search a bomb to beat it */
    for (rank = CARD_RANK_END - 1; rank >= CARD_RANK_BEG; rank--) {
      if (ctx->count[rank] == 4) {
        canbeat = 1;
        Hand_Clear(beat);
        CardArray_CopyRank(&beat->cards, &ctx->cards, (uint8_t)rank);
        break;
      }
    }
  }

  /* search for nuke */
  if (canbeat == 0) {
    if (ctx->count[CARD_RANK_r] && ctx->count[CARD_RANK_R]) {
      canbeat = 1;
      Hand_Clear(beat);
      beat->type =
          Hand_Format(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, HAND_CHAINLESS);
      CardArray_CopyRank(&beat->cards, &ctx->cards, CARD_RANK_R);
      CardArray_CopyRank(&beat->cards, &ctx->cards, CARD_RANK_r);
    }
  } else {
    beat->type =
        Hand_Format(HAND_PRIMAL_BOMB, HAND_KICKER_NONE, HAND_CHAINLESS);
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
static int
SearchBeat_TrioKicker(hand_ctx_t *ctx, hand_t *tobeat, hand_t *beat, int kick) {
  int rank = 0;
  int canbeat = 0;
  int triorank = 0;
  int kickrank = 0;
  card_array_t trio, kicker;
  hand_t htriobeat, hkickbeat;

  Hand_Clear(&htriobeat);
  Hand_Clear(&hkickbeat);
  Hand_Split(tobeat, &trio, &kicker);
  triorank = CARD_RANK(CardArray_At(&trio, 0));
  kickrank = CARD_RANK(CardArray_At(&kicker, 0));

  /* same rank trio , case b: keep the trio and search for a higher kicker */
  if (CardArray_IsContain(&ctx->rcards, &trio)) {
    for (rank = kickrank + 1; rank < CARD_RANK_END; rank++) {
      if ((rank != triorank) && (ctx->count[rank] >= kick)) {
        CardArray_Copy(&htriobeat.cards, &trio);
        CardArray_TakeRank(&hkickbeat.cards, &ctx->rcards, (uint8_t)rank, kick);
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
    if (SearchBeat_Primal(ctx, &trio, 0, &htriobeat, HAND_PRIMAL_TRIO)) {
      triorank = Hand_Rank(&htriobeat);

      for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
        if ((rank != triorank) && (ctx->count[rank] >= kick)) {
          CardArray_TakeRank(
              &hkickbeat.cards, &ctx->rcards, (uint8_t)rank, kick);
          canbeat = 1;
          break;
        }
      }
    }
  }

  /* beat */
  if (canbeat == 1) {
    Hand_Clear(beat);
    CardArray_Concat(&beat->cards, &htriobeat.cards);
    CardArray_Concat(&beat->cards, &hkickbeat.cards);
    beat->type = tobeat->type;
  }

  return canbeat;
}

/*
 * the lowest chain of the same length that starts above the chain in
 * `tobeat`, every rank held at least `duplicate` times
 */
static int SearchBeat_Chain(
    hand_ctx_t *ctx, const card_array_t *tobeat, int tobeattype, hand_t *beat,
    int duplicate) {
  int i, j;
  int chainlength = CardArray_Length(tobeat) / duplicate;
  int footer = CardArray_LowestRank(tobeat);

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
      Hand_Clear(beat);
      beat->type = (uint8_t)tobeattype;

      /* from the top of the chain down, the low suits of every rank */
      for (j = chainlength - 1; j >= 0; j--) {
        card_array_t rank;

        CardArray_Clear(&rank);
        CardArray_TakeRank(&rank, &ctx->rcards, (uint8_t)(i + j), duplicate);
        CardArray_Reverse(&rank);
        CardArray_Concat(&beat->cards, &rank);
      }

      return 1;
    }
  }

  return 0;
}

static int SearchBeat_TrioKickerChain(
    hand_ctx_t *ctx, hand_t *tobeat, hand_t *beat, int kc) {
  int canbeat = 0;
  int i, j, rank, chainlength;
  int triocount[CARD_RANK_END];
  int kickcount[CARD_RANK_END];
  int combrankmap[CARD_RANK_END];
  int rankcombmap[CARD_RANK_END];
  int comb[CARD_RANK_END];
  card_array_t trio, kicker;
  hand_t htriobeat, hkickbeat;

  Hand_Clear(&htriobeat);
  Hand_Clear(&hkickbeat);
  Hand_Split(tobeat, &trio, &kicker);
  chainlength = Hand_PrimalRanks(&trio);

  /* self beat, see SearchBeat_TrioKicker */
  if (CardArray_IsContain(&ctx->rcards, &trio)) {
    int n = 0; /* combination total */

    /* the ranks that can be kickers: not a trio of the chain, enough cards */
    CardArray_CountRanks(&trio, triocount);
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
    CardArray_CountRanks(&kicker, kickcount);

    for (rank = CARD_RANK_BEG; rank < CARD_RANK_END; rank++) {
      if (kickcount[rank] != 0)
        comb[j++] = rankcombmap[rank];
    }

    /* find next combination */
    if (LMath_NextComb(comb, chainlength, n)) {
      /* next combination found, copy kickers */
      for (i = 0; i < chainlength; i++)
        CardArray_TakeRank(
            &hkickbeat.cards, &ctx->rcards, (uint8_t)combrankmap[comb[i]], kc);

      canbeat = 1;

      /* copy trio to beat */
      CardArray_Concat(&htriobeat.cards, &trio);
      CardArray_Sort(&hkickbeat.cards);
    }
  }

  /* can't find same rank trio chain, search for higher rank trio */
  if (canbeat == 0) {
    /* higher rank trio chain found, search for the lowest kickers */
    if (SearchBeat_Chain(ctx, &trio, 0, &htriobeat, HAND_PRIMAL_TRIO)) {
      int kickers = 0;

      CardArray_CountRanks(&htriobeat.cards, triocount);

      for (rank = CARD_RANK_BEG;
           (rank < CARD_RANK_END) && (kickers < chainlength); rank++) {
        if ((triocount[rank] == 0) && (ctx->count[rank] >= kc)) {
          CardArray_TakeRank(&hkickbeat.cards, &ctx->rcards, (uint8_t)rank, kc);
          kickers++;
        }
      }

      if (kickers == chainlength)
        canbeat = 1;
    }
  }

  /* final */
  if (canbeat) {
    Hand_Clear(beat);
    CardArray_Concat(&beat->cards, &htriobeat.cards);
    CardArray_Concat(&beat->cards, &hkickbeat.cards);
    beat->type = tobeat->type;
  }

  return canbeat;
}

static int SearchBeat_Any(card_array_t *cards, hand_t *tobeat, hand_t *beat) {
  int canbeat = 0;
  hand_ctx_t ctx;

  /* setup search context */
  HandCtx_Setup(&ctx, cards);

  /* start search */
  switch (tobeat->type) {
  case Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAINLESS):
    canbeat = SearchBeat_Primal(
        &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_SOLO);
    break;

  case Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAINLESS):
    canbeat = SearchBeat_Primal(
        &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_PAIR);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAINLESS):
    canbeat = SearchBeat_Primal(
        &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_TRIO);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, HAND_CHAINLESS):
    canbeat = SearchBeat_TrioKicker(&ctx, tobeat, beat, HAND_PRIMAL_PAIR);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, HAND_CHAINLESS):
    canbeat = SearchBeat_TrioKicker(&ctx, tobeat, beat, HAND_PRIMAL_SOLO);
    break;

  case Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAIN):
    canbeat = SearchBeat_Chain(
        &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_SOLO);
    break;

  case Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAIN):
    canbeat = SearchBeat_Chain(
        &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_PAIR);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAIN):
    canbeat = SearchBeat_Chain(
        &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_TRIO);
    break;

  case Hand_Format(HAND_PRIMAL_FOUR, HAND_KICKER_NONE, HAND_CHAIN):
    canbeat = SearchBeat_Chain(
        &ctx, &tobeat->cards, tobeat->type, beat, HAND_PRIMAL_FOUR);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, HAND_CHAIN):
    canbeat = SearchBeat_TrioKickerChain(&ctx, tobeat, beat, HAND_PRIMAL_PAIR);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, HAND_CHAIN):
    canbeat = SearchBeat_TrioKickerChain(&ctx, tobeat, beat, HAND_PRIMAL_SOLO);
    break;

  default:
    break;
  }

  /* search for bomb/nuke */
  if (canbeat == 0)
    canbeat = SearchBeat_Bomb(&ctx, tobeat, beat);

  return canbeat;
}

int Beat_Search(card_array_t *cards, hand_t *tobeat, hand_t *beat) {
  /* already in search loop, continue */
  if (beat->type != 0)
    return SearchBeat_Any(cards, beat, beat);
  else
    return SearchBeat_Any(cards, tobeat, beat);
}

rk_list_t *Beat_SearchAll(card_array_t *cards, hand_t *tobeat) {
  rk_list_t *hl = NULL;
  hand_t htobeat;
  hand_t beat;
  hand_t judged;
  int canbeat = 0;

  Hand_Clear(&beat);
  Hand_Copy(&htobeat, tobeat);

  hl = rk_list_create();
  do {
    canbeat = SearchBeat_Any(cards, &htobeat, &beat);

    if (canbeat) {
      /* keep searching from this one, whatever the rules say about it */
      Hand_Copy(&htobeat, &beat);

      /* the search proposes, the rules decide */
      if ((Hand_Parse(&judged, &beat.cards) != HAND_NONE) &&
          (Hand_Compare(&judged, tobeat) == HAND_CMP_GREATER))
        HandList_PushFront(hl, &beat);
    }
  } while (canbeat);

  return hl;
}
