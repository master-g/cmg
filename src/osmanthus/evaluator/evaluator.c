/*
 *  evaluator.c
 *  evaluator
 *
 *  Created by Master.G on 13-7-25.
 *  Copyright (c) 2013 MED. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>

#include "evalarrays.h"
#include "evaluator.h"

/* Poker hand evaluator
 *
 * Kevin L. Suffecool
 * suffecool@bigfoot.com
 *
 
 *
 *   This routine initializes the deck.  A deck of cards is
 *   simply an integer array of length 52 (no jokers).  This
 *   array is populated with each card, using the following
 *   scheme:
 *
 *   An integer is made up of four bytes.  The high-order
 *   bytes are used to hold the rank bit pattern, whereas
 *   the low-order bytes hold the suit/rank/prime value
 *   of the card.
 *
 *   +--------+--------+--------+--------+
 *   |xxxbbbbb|bbbbbbbb|cdhsrrrr|xxpppppp|
 *   +--------+--------+--------+--------+
 *
 *   p = prime number of rank (deuce=2,trey=3,four=5,five=7,...,ace=41)
 *   r = rank of card (deuce=0,trey=1,four=2,five=3,...,ace=12)
 *   cdhs = suit of card
 *   b = bit turned on depending on rank of card
 */
void eval_init_deck(int *deck) {
  int i, j, n = 0, suit = 0x8000;

  for (i = 0; i < 4; i++, suit >>= 1)
    for (j = 0; j < 13; j++, n++)
      deck[n] = eval_primes[j] | (j << 8) | suit | (1 << (16 + j));
}

/*
 This routine takes a deck and randomly mixes up
 the order of the cards.
 */
void eval_shuffle_deck(int *deck) {
  int i, n, temp[52];

  for (i = 0; i < 52; i++)
    temp[i] = deck[i];

  for (i = 0; i < 52; i++) {
    do {
      n = (int)(51.9999999 * drand48());
    } while (temp[n] == 0);
    deck[i] = temp[n];
    temp[n] = 0;
  }
}

int eval_hand_rank(short val) {
  if (val > 467) return (HIGH_CARD); /*  274 high cards      */
  if (val > 311) return (PAIR); /*  156 one pairs       */
  if (val > 299) return (STRAIGHT); /*  12 straights        */
  if (val > 25) return (FLUSH); /*  274 flushes         */
  if (val > 13) return (STRAIGHT_FLUSH); /*  12 straight flushes */
  return (LEOPARD); /*  13 Leopard          */
}

const char *eval_hand_rank_description(short val) {
  char *hand_descriptions[] =
      {
          "",
          "Leopard",
          "Straight Flush",
          "Flush",
          "Straight",
          "Pair",
          "High Card"};

  return hand_descriptions[val];
}

#ifdef EVAL_USE_HASH
/* use hash to compress lookup table size */
unsigned int eval_hash_prime_product(unsigned int val) {
  unsigned int a, b, rsl;

  val += 0xf382e87a;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b   = (val >> 3) & 0x1f;
  a   = (val + (val << 11)) >> 24;
  rsl = (a ^ eval_hash_table[b]);

  return rsl;
}
#endif

unsigned int eval_3hand(int c1, int c2, int c3) {
  int s;
  int q     = (c1 | c2 | c3) >> 16;
  int prime = (eval_primes[RANK(c1)] * eval_primes[RANK(c2)] * eval_primes[RANK(c3)]);

  if (c1 & c2 & c3 & 0xf000)
    return eval_flushes[q]; /* check for flushes and straight flushes */

  if ((s = eval_unique3[q]) != 0)
    return s; /* check for straights and high hands */
  else
#ifndef EVAL_USE_HASH
    return eval_others[prime];
#else
    return eval_hash_others[eval_hash_prime_product(prime)];
#endif
}

unsigned int eval_card_make(int suit, int rank) {
  unsigned int card     = 0;
  unsigned int rank_bit = 0x010000;
  unsigned int suit_bit = 0;
  unsigned int rank_num = 0;
  unsigned int prime    = 0;

  rank_bit = rank_bit << rank;
  suit_bit = suit;
  rank_num = rank << 8;
  prime    = eval_primes[rank];

  card |= prime;
  card |= rank_num;
  card |= suit_bit;
  card |= rank_bit;

  return card;
}