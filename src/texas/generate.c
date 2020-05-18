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

#include "bits.h"
#include "generate.h"

#define MAGIC_FLUSH_START 323

#define MAGIC_STRAIGHT_FLUSH_START 1
#define MAGIC_FOUR_OF_A_KIND_START 11
#define MAGIC_FULLHOUSE_START 167
#define MAGIC_STRAIGHT_START 1600
#define MAGIC_THREE_OF_A_KIND_START 1610
#define MAGIC_TWO_PAIR_START 2468
#define MAGIC_ONE_PAIR_START 3326
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

#define DEBUG_ENABLE 0

#if (DEBUG_ENABLE == 1)
#include <stdio.h>
static const char s_debug_prime2card[] = {
    ' ', // 0
    ' ', // 1
    '2', // 2
    '3', // 3
    ' ', // 4
    '4', // 5
    ' ', // 6
    '5', // 7
    ' ', // 8
    ' ', // 9
    ' ', // 10
    '6', // 11
    ' ', // 12
    '7', // 13
    ' ', // 14
    ' ', // 15
    ' ', // 16
    '8', // 17
    ' ', // 18
    '9', // 19
    ' ', // 20
    ' ', // 21
    ' ', // 22
    'T', // 23
    ' ', // 24
    ' ', // 25
    ' ', // 26
    ' ', // 27
    ' ', // 28
    'J', // 29
    ' ', // 30
    'Q', // 31
    ' ', // 32
    ' ', // 33
    ' ', // 34
    ' ', // 35
    ' ', // 36
    'K', // 37
    ' ', // 38
    ' ', // 39
    ' ', // 40
    'A', // 41
};
#endif

void kv_push(texas_magic_kv_t **list, uint32_t product, uint16_t magic) {
  texas_magic_kv_t *new_head =
      (texas_magic_kv_t *)malloc(sizeof(texas_magic_kv_t));
  new_head->product = product;
  new_head->magic = magic;
  new_head->next = *list;

  *list = new_head;
}

texas_magic_kv_t *kv_sorted_merge(texas_magic_kv_t *a, texas_magic_kv_t *b) {
  if (a == NULL) {
    return b;
  } else if (b == NULL) {
    return a;
  }

  texas_magic_kv_t *result = NULL;

  if (a->magic <= b->magic) {
    result = a;
    result->next = kv_sorted_merge(a->next, b);
  } else {
    result = b;
    result->next = kv_sorted_merge(a, b->next);
  }

  return result;
}

void kv_front_back_split(texas_magic_kv_t *source, texas_magic_kv_t **front_ref,
                         texas_magic_kv_t **back_ref) {
  if (source == NULL || source->next == NULL) {
    *front_ref = source;
    *back_ref = NULL;
    return;
  }

  texas_magic_kv_t *slow = source;
  texas_magic_kv_t *fast = source->next;

  while (fast != NULL) {
    fast = fast->next;
    if (fast != NULL) {
      slow = slow->next;
      fast = fast->next;
    }
  }

  *front_ref = source;
  *back_ref = slow->next;
  slow->next = NULL;
}

void kv_merge_sort(texas_magic_kv_t **head) {
  if (*head == NULL || (*head)->next == NULL) {
    return;
  }

  texas_magic_kv_t *a;
  texas_magic_kv_t *b;

  kv_front_back_split(*head, &a, &b);

  kv_merge_sort(&a);
  kv_merge_sort(&b);

  *head = kv_sorted_merge(a, b);
}

// four-of-a-kind, full-house
void texas_gen_2(texas_magic_kv_t **list, int coef1, int coef2,
                 uint16_t magic_start) {

  uint16_t magic = magic_start;

  for (int i = 0; i < s_num_primes; i++) {
    for (int j = 0; j < s_num_primes; j++) {
      if (i == j) {
        continue;
      }

      uint32_t product = 1;
      for (int t = 0; t < coef1; t++) {
        product *= s_primes[i];
      }
      for (int t = 0; t < coef2; t++) {
        product *= s_primes[j];
      }

      kv_push(list, product, magic);
      magic++;
    }
  }
}

void texas_gen_311(texas_magic_kv_t **list, uint16_t magic_start) {
  uint16_t magic = magic_start;
  for (int i = 0; i < s_num_primes; i++) {
    for (int j = 0; j < s_num_primes; j++) {
      for (int k = j + 1; k < s_num_primes; k++) {
        if (i == j || i == k) {
          continue;
        }

        uint32_t product =
            s_primes[i] * s_primes[i] * s_primes[i] * s_primes[j] * s_primes[k];

#if (DEBUG_ENABLE == 1)
        char c1 = s_debug_prime2card[s_primes[i]];
        char c2 = s_debug_prime2card[s_primes[j]];
        char c3 = s_debug_prime2card[s_primes[k]];
        printf("%c%c%c%c%c -> p:%d, m:%d\n", c1, c1, c1, c2, c3, product,
               magic);
#endif

        kv_push(list, product, magic);
        magic++;
      }
    }
  }
}

void texas_gen_221(texas_magic_kv_t **list, uint16_t magic_start) {
  uint16_t magic = magic_start;
  for (int i = 0; i < s_num_primes; i++) {
    for (int j = i + 1; j < s_num_primes; j++) {
      for (int k = 0; k < s_num_primes; k++) {
        if (i == k || j == k) {
          continue;
        }

        uint32_t product =
            s_primes[i] * s_primes[i] * s_primes[j] * s_primes[j] * s_primes[k];

#if (DEBUG_ENABLE == 1)
        char c1 = s_debug_prime2card[s_primes[i]];
        char c2 = s_debug_prime2card[s_primes[j]];
        char c3 = s_debug_prime2card[s_primes[k]];
        printf("%c%c%c%c%c -> p:%d, m:%d\n", c1, c1, c2, c2, c3, product,
               magic);
#endif

        kv_push(list, product, magic);
        magic++;
      }
    }
  }
}

void texas_gen_2111(texas_magic_kv_t **list, uint16_t magic_start) {
  uint16_t magic = magic_start;
  for (int i = 0; i < s_num_primes; i++) {
    for (int j = 0; j < s_num_primes; j++) {
      for (int k = j + 1; k < s_num_primes; k++) {
        for (int m = k + 1; m < s_num_primes; m++) {
          if (i == j || i == k || i == m) {
            continue;
          }
          uint32_t a = s_primes[i];
          uint32_t b = s_primes[j];
          uint32_t c = s_primes[k];
          uint32_t d = s_primes[m];

          uint32_t product = a * a * b * c * d;

#if (DEBUG_ENABLE == 1)
          char c1 = s_debug_prime2card[s_primes[i]];
          char c2 = s_debug_prime2card[s_primes[j]];
          char c3 = s_debug_prime2card[s_primes[k]];
          char c4 = s_debug_prime2card[s_primes[m]];
          printf("%c%c%c%c%c -> p:%d, m:%d\n", c1, c1, c2, c3, c4, product,
                 magic);
#endif

          kv_push(list, product, magic);
          magic++;
        }
      }
    }
  }
}

void texas_gen_others(texas_magic_kv_t **list) {
  texas_gen_2(list, 4, 1, MAGIC_FOUR_OF_A_KIND_START);
  texas_gen_2(list, 3, 2, MAGIC_FULLHOUSE_START);
  texas_gen_311(list, MAGIC_THREE_OF_A_KIND_START);
  texas_gen_221(list, MAGIC_TWO_PAIR_START);
  texas_gen_2111(list, MAGIC_ONE_PAIR_START);

  // mph does not require key to be sorted
  kv_merge_sort(list);
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
