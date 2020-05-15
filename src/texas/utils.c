//
// Created by MasterG on 2020/5/15.
//

#include <stddef.h>
#include <stdint.h>

int texas_init_deck(uint32_t *deck) {
  uint16_t primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};

  if (deck != NULL) {
    uint32_t i, j, n = 0;
    uint32_t suit = 0x8000;

    for (i = 0; i < 4; i++, suit >>= 1u) {
      for (j = 0; j < 13; j++, n++) {
        deck[n] = primes[j] | (j << 8u) | suit | (1u << (16 + j));
      }
    }
  }

  return 52;
}
