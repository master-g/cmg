//
// Created by MasterG on 2020/5/13.
//

/**
 * +--------+--------+--------+--------+
 * |xxxbbbbb|bbbbbbbb|cdhsrrrr|xxpppppp|
 * +--------+--------+--------+--------+
 *
 * p = prime number of rank (deuce=2,trey=3,four=5,five=7,...,ace=41)
 * r = rank of card (deuce=0,trey=1,four=2,five=3,...,ace=12)
 * cdhs = suit of card
 * b = bit turned on depending on rank of card
 */

#include <stddef.h>
#include "generate.h"

static uint32_t sg_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};

int texas_init_deck(uint32_t *deck) {
  if (deck != NULL) {
    uint32_t i, j, n = 0;
    uint32_t suit = 0x8000;

    for (i = 0; i < 4; i++, suit >>= 1u) {
      for (j = 0; j < 13; j++, n++) {
        deck[n] = sg_primes[j] | (j << 8u) | suit | (1u << (16 + j));
      }
    }
  }

  return 52;
}
