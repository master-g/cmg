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
#include <stdlib.h>
#include <string.h>

#include "bits.h"
#include "generate.h"

#define MAGIC_FLUSH_START 323

#define MAGIC_STRAIGHT_FLUSH_START 1
#define MAGIC_FOUR_OF_A_KIND_START 11
#define MAGIC_STRAIGHT_START 1600
#define MAGIC_HIGH_CARD_START 6186

static const uint16_t s_primes[] = {41, 37, 31, 29, 23, 19, 17,
                                    13, 11, 7,  5,  3,  2};
static const int s_num_primes = sizeof(s_primes) / sizeof(s_primes[0]);

static const uint16_t s_straight_bits[] = {
    // AKQJ10 .... 65432, 5432A
    0x1F00, 0x0F80, 0x07C0, 0x03E0, 0x01F0,
    0x00F8, 0x007C, 0x003E, 0x001F, 0x100F,
};
static const int s_num_straights =
    sizeof(s_straight_bits) / sizeof(s_straight_bits[0]);

int texas_gen_flushes(uint16_t *arr) {
  if (arr == NULL) {
    return MAGIC_UNIQUE5_SIZE;
  }

  uint16_t highest = 0x1F00; // five cards, xxx11111 00000000
  uint16_t lowest = 0x1F;    // five cards, xxx00000 00011111
  uint16_t magic_flush = MAGIC_FLUSH_START;
  for (uint16_t c = highest; c >= lowest; c--) {
    // must be five cards
    if (texas_bits_ones_count_16(c) == 5) {
      int is_straight = 0;
      for (int j = 0; j < s_num_straights; j++) {
        if (c == s_straight_bits[j]) {
          is_straight = 1;
          arr[c] = j + MAGIC_STRAIGHT_FLUSH_START;
          break;
        }
      }

      if (is_straight == 0) {
        arr[c] = magic_flush;
        magic_flush++;
      }
    }
  }

  return MAGIC_UNIQUE5_SIZE;
}

int texas_gen_unique5(uint16_t *arr) {
  if (arr == NULL) {
    return MAGIC_UNIQUE5_SIZE;
  }

  // straights
  uint16_t highest = 0x1F00; // five cards, xxx11111 00000000
  uint16_t lowest = 0x1F;    // five cards, xxx00000 00011111
  uint16_t magic_high = MAGIC_HIGH_CARD_START;
  for (uint16_t c = highest; c >= lowest; c--) {
    // must be five cards
    if (texas_bits_ones_count_16(c) == 5) {
      int is_straight = 0;
      for (int j = 0; j < s_num_straights; j++) {
        if (c == s_straight_bits[j]) {
          is_straight = 1;
          arr[c] = j + MAGIC_STRAIGHT_START;
          break;
        }
      }

      if (is_straight == 0) {
        arr[c] = magic_high;
        magic_high++;
      }
    }
  }

  return MAGIC_UNIQUE5_SIZE;
}

int next_comb(int comb[], int k, int n) {
  int i = k - 1;
  ++comb[i];
  while ((i > 0) && (comb[i] >= n - k + 1 + i)) {
    --i;
    /* printf(">>>>>>>>> %d\n", i); */
    ++comb[i];
  }

  if (comb[0] > n - k) /* Combination (n-k, n-k+1, ..., n) reached */
    return 0;          /* No more combinations can be generated */

  /* comb now looks like (..., x, n, n, n, ..., n).
     Turn it into (..., x, x + 1, x + 2, ...) */
  for (i = i + 1; i < k; ++i)
    comb[i] = comb[i - 1] + 1;

  return 1;
}

void add_to_list(texas_magic_kv_t **list, uint16_t product, uint16_t magic) {
  texas_magic_kv_t *entry =
      (texas_magic_kv_t *)malloc(sizeof(texas_magic_kv_t));
  entry->product = product;
  entry->magic = magic;

  if (*list == NULL) {
    *list = entry;
    entry->next = NULL;
  } else {
    entry->next = (*list)->next;
    (*list)->next = entry;
  }
}

#include <stdio.h>
// four-of-a-kind, full-house
void texas_gen_2(texas_magic_kv_t **list, int coef1, int coef2,
                 uint16_t magic_start) {

  uint16_t magic = magic_start;

  for (int i = 0; i < s_num_primes; i++) {
    for (int j = 0; j < s_num_primes; j++) {
      if (i == j) {
        continue;
      }

      int product = 1;
      for (int t = 0; t < coef1; t++) {
        product *= s_primes[i];
      }
      for (int t = 0; t < coef2; t++) {
        product *= s_primes[j];
      }

      printf("%d^4 * %d = %d\n", s_primes[i], s_primes[j], product);

      add_to_list(list, product, magic);
      magic++;
    }
  }
}

void texas_gen_others(texas_magic_kv_t **list) {
  texas_gen_2(list, 4, 1, MAGIC_FOUR_OF_A_KIND_START);
}

void texas_free_others(texas_magic_kv_t *list) {
  if (list == NULL) {
    return;
  }

  texas_magic_kv_t *head = list;
  while (head != NULL) {
    texas_magic_kv_t *next = head->next;
    free(head);
    head = next;
  }
}
