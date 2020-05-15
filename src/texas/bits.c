//
// Created by MasterG on 2020/5/15.
//

#include <stddef.h>

#include "bits.h"

int texas_bits_str_32(uint32_t v, char *buf) {
  if (buf == NULL) {
    return 32;
  }

  for (int i = 0; i < 32; i++) {
    uint32_t bit = (v >> (uint32_t)(31 - i)) & 0x1u;
    buf[i] = (char)('0' + bit);
  }

  return 32;
}

int texas_bits_str_16(uint16_t v, char *buf) {
  if (buf == NULL) {
    return 16;
  }

  for (int i = 0; i < 16; i++) {
    uint16_t bit = v >> (uint16_t)(15 - i);
    bit &= 0x1u;
    buf[i] = (char)('0' + bit);
  }

  return 16;
}

int texas_bits_ones_count_32(uint32_t v) {
  if (v == 0) {
    return 0;
  }

  int count = 0;
  for (int i = 0; i < 32; i++) {
    uint32_t bit = (v >> (uint32_t)(31 - i)) & 0x1u;
    count += bit == 0 ? 0 : 1;
  }

  return count;
}

int texas_bits_ones_count_16(uint16_t v) {
  if (v == 0) {
    return 0;
  }

  int count = 0;
  for (int i = 0; i < 16; i++) {
    uint16_t bit = v >> (uint16_t)(15 - i);
    bit &= 0x1u;
    count += bit == 0 ? 0 : 1;
  }

  return count;
}
