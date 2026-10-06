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
  /* original cards */
  card_array_t cards;
  /* reverse sorted cards */
  card_array_t rcards;

} hand_ctx_t;

#define HandCtx_Clear(ctx) memset((ctx), 0, sizeof(hand_ctx_t))

static void HandCtx_Setup(hand_ctx_t *ctx, card_array_t *array) {
  /* setup search context */
  HandCtx_Clear(ctx);

  Hand_CountRank(array, ctx->count);
  CardArray_Copy(&ctx->cards, array);
  CardArray_Copy(&ctx->rcards, array);
  CardArray_Sort(&ctx->cards, NULL);
  CardArray_Reverse(&ctx->rcards);
}

static int
SearchBeat_Primal(hand_ctx_t *ctx, hand_t *tobeat, hand_t *beat, int primal) {
  int i = 0;
  int canbeat = 0;
  int *count = NULL;
  int rank = 0;
  card_array_t *temp = NULL;
  int tobeattype = tobeat->type;

  count = ctx->count;
  temp = &ctx->rcards;

  rank = CARD_RANK(tobeat->cards.cards[0]);

  /* search for primal */
  for (i = 0; i < temp->length;) {
    int c = count[CARD_RANK(temp->cards[i])];
    if ((CARD_RANK(temp->cards[i]) > rank) && c >= primal) {
      Hand_Clear(beat);
      beat->type = (uint8_t)tobeattype;
      CardArray_PushBackCards(&beat->cards, temp, i, primal);
      canbeat = 1;
      break;
    }
    i += c;
  }

  return canbeat;
}

static int SearchBeat_Bomb(hand_ctx_t *ctx, hand_t *tobeat, hand_t *beat) {
  int canbeat = 0;
  int *count = NULL;
  int i = 0;
  card_array_t *cards = &ctx->cards;

  count = ctx->count;

  /*
   * This only decides where to look, Beat_SearchAll asks the rules
   * whether what was found really beats the hand.
   */
  if (Hand_IsNuke(tobeat))
    return 0;

  /* search for a higher rank bomb */
  if (Hand_IsBomb(tobeat)) {
    canbeat = SearchBeat_Primal(ctx, tobeat, beat, 4);
  } else {
    /* tobeat is not a nuke or bomb, search a bomb to beat it */
    for (i = 0; i < ctx->cards.length;) {
      int c = count[CARD_RANK(ctx->cards.cards[i])];
      if (c == 4) {
        canbeat = 1;
        Hand_Clear(beat);
        CardArray_CopyRank(&beat->cards, cards, CARD_RANK(ctx->cards.cards[i]));
        break;
      }
      i += c;
    }
  }

  /* search for nuke */
  if (canbeat == 0) {
    if (count[CARD_RANK_r] && count[CARD_RANK_R]) {
      canbeat = 1;
      Hand_Clear(beat);
      beat->type =
          Hand_Format(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, HAND_CHAINLESS);
      CardArray_CopyRank(&beat->cards, cards, CARD_RANK_R);
      CardArray_CopyRank(&beat->cards, cards, CARD_RANK_r);
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
  int i = 0;
  int canbeat = 0;
  int cantriobeat = 0;
  int tobeattype = tobeat->type;
  int *count = NULL;
  card_array_t temp;
  hand_t htrio, hkick, htriobeat, hkickbeat;

  Hand_Clear(&htrio);
  Hand_Clear(&hkick);
  Hand_Clear(&htriobeat);
  Hand_Clear(&hkickbeat);

  count = ctx->count;
  CardArray_Copy(&temp, &ctx->rcards);

  /* copy hands */
  CardArray_PushBackCards(&htrio.cards, &tobeat->cards, 0, 3);
  CardArray_PushBackCards(&hkick.cards, &tobeat->cards, 3, kick);

  /* same rank trio , case b */
  if (CardArray_IsContain(&temp, &htrio.cards)) {
    /* keep trio beat */
    CardArray_Copy(&htriobeat.cards, &htrio.cards);
    CardArray_RemoveRank(&temp, CARD_RANK(htriobeat.cards.cards[0]));

    /* search for a higher kicker */
    /* round 1: only search those count[rank] == kick */
    for (i = 0; i < temp.length;) {
      int c = count[CARD_RANK(temp.cards[i])];
      if (c >= kick &&
          CARD_RANK(temp.cards[i]) > CARD_RANK(hkick.cards.cards[0])) {
        CardArray_Clear(&hkickbeat.cards);
        CardArray_PushBackCards(&hkickbeat.cards, &temp, i, kick);
        canbeat = 1;
        break;
      }
      i += c;
    }

    /* if kicker can't beat, restore trio */
    if (canbeat == 0) {
      CardArray_Clear(&htriobeat.cards);
      CardArray_Copy(&temp, &ctx->rcards);
    }
  }

  /*
   * same rank trio not found
   * OR
   * same rank trio found, but kicker can't beat
   */
  if (canbeat == 0) {
    cantriobeat = SearchBeat_Primal(ctx, &htrio, &htriobeat, HAND_PRIMAL_TRIO);

    /* trio beat found, search for kicker beat */
    if (cantriobeat == 1) {
      /* remove trio from temp */
      CardArray_RemoveRank(&temp, CARD_RANK(htriobeat.cards.cards[0]));

      /* search for a kicker */
      for (i = 0; i < temp.length;) {
        int c = count[CARD_RANK(temp.cards[i])];
        if (c >= kick) {
          CardArray_PushBackCards(&hkickbeat.cards, &temp, i, kick);
          canbeat = 1;
          break;
        }
        i += c;
      }
    }
  }

  /* beat */
  if (canbeat == 1) {
    Hand_Clear(beat);
    CardArray_Concat(&beat->cards, &htriobeat.cards);
    CardArray_Concat(&beat->cards, &hkickbeat.cards);
    beat->type = (uint8_t)tobeattype;
  }

  return canbeat;
}

static int
SearchBeat_Chain(hand_ctx_t *ctx, hand_t *tobeat, hand_t *beat, int duplicate) {
  int canbeat = 0;
  int found = 0;
  int i, j, k, chainlength;
  int tobeattype = tobeat->type;
  uint8_t footer = 0;
  int *count = NULL;
  card_array_t *cards = &ctx->cards;
  card_array_t temp;

  count = ctx->count;
  CardArray_Clear(&temp);

  chainlength = tobeat->cards.length / duplicate;
  footer = CARD_RANK(tobeat->cards.cards[tobeat->cards.length - 1]);

  /* search for beat chain in rank counts */
  for (i = footer + 1; i <= CARD_RANK_2 - chainlength; i++) {
    found = 1;

    for (j = 0; j < chainlength; j++) {
      /* check if chain breaks */
      if (count[i + j] < duplicate) {
        found = 0;
        break;
      }
    }

    if (found) {
      footer = (uint8_t)i; /* beat footer rank */
      k = duplicate;       /* how many cards needed for each rank */

      for (j = cards->length - 1; j >= 0 && chainlength > 0; j--) {
        if (CARD_RANK(cards->cards[j]) == footer) {
          CardArray_PushFront(&temp, cards->cards[j]);
          k--;

          if (k == 0) {
            k = duplicate;
            chainlength--;
            footer++;
          }
        }
      }

      break;
    }
  }

  if (found) {
    beat->type = (uint8_t)tobeattype;
    CardArray_Copy(&beat->cards, &temp);
    canbeat = 1;
  }

  return canbeat;
}

static int SearchBeat_TrioKickerChain(
    hand_ctx_t *ctx, hand_t *tobeat, hand_t *beat, int kc) {
  int canbeat = 0;
  int cantriobeat = 0;
  int i, j, chainlength;
  int tobeattype = tobeat->type;
  int count[CARD_RANK_END];
  int kickcount[CARD_RANK_END];
  int combrankmap[CARD_RANK_END];
  int rankcombmap[CARD_RANK_END];
  int comb[CARD_RANK_END];
  card_array_t temp;
  hand_t htrio, hkick, htriobeat, hkickbeat;

  /* setup variables */
  memcpy(count, ctx->count, sizeof(int) * CARD_RANK_END);

  Hand_Clear(&htrio);
  Hand_Clear(&hkick);
  Hand_Clear(&htriobeat);
  Hand_Clear(&hkickbeat);

  CardArray_Copy(&temp, &ctx->rcards);
  chainlength = tobeat->cards.length / (HAND_PRIMAL_TRIO + kc);

  /* copy tobeat cards */
  CardArray_PushBackCards(&htrio.cards, &tobeat->cards, 0, 3 * chainlength);
  CardArray_PushBackCards(
      &hkick.cards, &tobeat->cards, 3 * chainlength, chainlength * kc);

  htrio.type = Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAIN);

  /* self beat, see SearchBeat_TrioKicker */
  if (CardArray_IsContain(&temp, &htrio.cards)) {
    int n = 0; /* combination total */

    /* remove trio from kickcount */
    memcpy(kickcount, count, sizeof(int) * CARD_RANK_END);

    for (i = 0; i < htrio.cards.length; i += 3)
      kickcount[CARD_RANK(htrio.cards.cards[i])] = 0;

    /* remove count < kc and calculate n */
    for (i = CARD_RANK_3; i < CARD_RANK_END; i++) {
      if (kickcount[i] < kc)
        kickcount[i] = 0;
      else
        n++;
    }

    /* setup comb-rank and rank-comb map */
    j = 0;
    memset(combrankmap, -1, sizeof(int) * CARD_RANK_END);
    memset(rankcombmap, -1, sizeof(int) * CARD_RANK_END);

    for (i = CARD_RANK_3; i < CARD_RANK_END; i++) {
      if (kickcount[i] != 0) {
        combrankmap[j] = i;
        rankcombmap[i] = j;
        j++;
      }
    }

    /* setup combination */
    j = 0;
    memset(comb, -1, sizeof(int) * CARD_RANK_END);

    for (i = 0; i < hkick.cards.length; i += kc)
      comb[j++] = rankcombmap[CARD_RANK(hkick.cards.cards[i])];

    /*
     * LMath_NextComb needs it ascending, the kickers come in whatever order
     * the previous search left them
     */
    for (i = 1; i < chainlength; i++) {
      int key = comb[i];

      for (j = i; j > 0 && comb[j - 1] > key; j--)
        comb[j] = comb[j - 1];

      comb[j] = key;
    }

    /* find next combination */
    if (LMath_NextComb(comb, chainlength, n)) {
      /* next combination found, copy kickers */
      for (i = 0; i < chainlength; i++) {
        int rank = combrankmap[comb[i]];

        for (j = 0; j < temp.length; j++) {
          if (CARD_RANK(temp.cards[j]) == rank) {
            CardArray_PushBackCards(&hkickbeat.cards, &temp, j, kc);
            break;
          }
        }
      }

      canbeat = 1;

      /* copy trio to beat */
      CardArray_Concat(&htriobeat.cards, &htrio.cards);
      CardArray_Sort(&hkickbeat.cards, NULL);
    }
  }

  /* can't find same rank trio chain, search for higher rank trio */
  if (canbeat == 0) {
    /* restore rank count */
    memcpy(count, ctx->count, sizeof(int) * CARD_RANK_END);

    cantriobeat = SearchBeat_Chain(ctx, &htrio, &htriobeat, 3);

    /* higher rank trio chain found, search for kickers */
    if (cantriobeat) {
      /* remove trio from temp */
      for (i = 0; i < htriobeat.cards.length; i += 3) {
        CardArray_RemoveRank(&temp, CARD_RANK(htriobeat.cards.cards[i]));
        count[CARD_RANK(htriobeat.cards.cards[0])] = 0;
      }

      for (j = 0; j < chainlength; j++) {
        for (i = 0; i < temp.length; i++) {
          if (count[CARD_RANK(temp.cards[i])] >= kc) {
            CardArray_PushBackCards(&hkickbeat.cards, &temp, i, kc);
            CardArray_RemoveRank(&temp, CARD_RANK(temp.cards[i]));
            break;
          }
        }
      }

      if (hkickbeat.cards.length == kc * chainlength)
        canbeat = 1;
    }
  }

  /* final */
  if (canbeat) {
    Hand_Clear(beat);
    CardArray_Concat(&beat->cards, &htriobeat.cards);
    CardArray_Concat(&beat->cards, &hkickbeat.cards);
    beat->type = (uint8_t)tobeattype;
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
    canbeat = SearchBeat_Primal(&ctx, tobeat, beat, HAND_PRIMAL_SOLO);
    break;

  case Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAINLESS):
    canbeat = SearchBeat_Primal(&ctx, tobeat, beat, HAND_PRIMAL_PAIR);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAINLESS):
    canbeat = SearchBeat_Primal(&ctx, tobeat, beat, HAND_PRIMAL_TRIO);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, HAND_CHAINLESS):
    canbeat = SearchBeat_TrioKicker(&ctx, tobeat, beat, HAND_PRIMAL_PAIR);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, HAND_CHAINLESS):
    canbeat = SearchBeat_TrioKicker(&ctx, tobeat, beat, HAND_PRIMAL_SOLO);
    break;

  case Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAIN):
    canbeat = SearchBeat_Chain(&ctx, tobeat, beat, HAND_PRIMAL_SOLO);
    break;

  case Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAIN):
    canbeat = SearchBeat_Chain(&ctx, tobeat, beat, HAND_PRIMAL_PAIR);
    break;

  case Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAIN):
    canbeat = SearchBeat_Chain(&ctx, tobeat, beat, HAND_PRIMAL_TRIO);
    break;

  case Hand_Format(HAND_PRIMAL_FOUR, HAND_KICKER_NONE, HAND_CHAIN):
    canbeat = SearchBeat_Chain(&ctx, tobeat, beat, HAND_PRIMAL_FOUR);
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
