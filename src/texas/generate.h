//
// Created by MasterG on 2020/5/13.
//

#ifndef CMG_TEXAS_GENERATE_H
#define CMG_TEXAS_GENERATE_H

#include <stdint.h>

#define MAGIC_UNIQUE5_SIZE 0x1F01   // (0001 1111 0000 0000) + 1
#define MAGIC_OTHERS_SIZE 104553157 // AAAAK 41^4 * 37, 'short' will take 200MB

typedef struct texas_magic_kv_s {
  uint16_t product;
  uint16_t magic;
  struct texas_magic_kv_s *next;
} texas_magic_kv_t;

int texas_gen_flushes(uint16_t *arr);

int texas_gen_unique5(uint16_t *arr);

void texas_gen_others(texas_magic_kv_t **list);

void texas_free_others(texas_magic_kv_t *list);

#endif // CMG_TEXAS_GENERATE_H
