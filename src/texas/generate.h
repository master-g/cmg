//
// Created by MasterG on 2020/5/13.
//

#ifndef CMG_TEXAS_GENERATE_H
#define CMG_TEXAS_GENERATE_H

#include <stdint.h>

#define MAGIC_UNIQUE5_SIZE 0x1F01 // (0001 1111 0000 0000) + 1

// std::list<std::pair<uint32_t, uint16_t>>
typedef struct texas_magic_kv_s {
  uint32_t product; // product of the cards' prime
  uint16_t magic;   // rank of the hand
  struct texas_magic_kv_s *next;
} texas_magic_kv_t;

int texas_gen_flushes(uint16_t *arr);

int texas_gen_unique5(uint16_t *arr);

void texas_gen_others(texas_magic_kv_t **list);

void texas_free_others(texas_magic_kv_t *list);

#endif // CMG_TEXAS_GENERATE_H
