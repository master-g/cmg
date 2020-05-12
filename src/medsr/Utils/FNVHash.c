/*
 *  FNVHash.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-17.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "FNVHash.h"

#define HASH_BASIS 2166136261U
#define HASH_PRIME 16777619U

/* Fowler/Noll/Vo hash algorithm */
uint32_t FNVHash(const char *data, int len) {
  uint32_t hash = HASH_BASIS;
  char *p = (char *)data;
  int i = 0;

  for (i = 0; i < len; i++)
    hash = (hash * HASH_PRIME) ^ (uint32_t)p[i];

  return hash;
}
