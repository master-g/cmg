/*
 *  pd.c
 *  DSAAC
 *
 *  Created by Master.G on 13-8-18.
 *  Copyright (c) 2013 Master.G. All rights reserved.
 */

#include "payload.h"

#include <assert.h>
#include <float.h>

enum ptype {
  PDATA_I8 = 0,
  PDATA_U8,
  PDATA_I16,
  PDATA_U16,
  PDATA_I32,
  PDATA_U32,
  PDATA_I64,
  PDATA_U64,
  PDATA_FLOAT,
  PDATA_DOUBLE,
  PDATA_STR,
  PDATA_RAW,
  PDATA_USER_DEFINED,

  PayloadTypeCount
};

pdata *pdata_from_i8(const uint8_t data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_I8;
  pd->size = sizeof(int8_t);
  pd->data.i8 = data;
  return pd;
}

pdata *pdata_from_u8(const uint8_t data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_U8;
  pd->size = sizeof(uint8_t);
  pd->data.u8 = data;
  return pd;
}

pdata *pdata_from_i16(const uint16_t data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_I16;
  pd->size = sizeof(int16_t);
  pd->data.i16 = data;
  return pd;
}

pdata *pdata_from_u16(const uint16_t data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_U16;
  pd->size = sizeof(uint16_t);
  pd->data.u16 = data;
  return pd;
}

pdata *pdata_from_i32(const uint32_t data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_I32;
  pd->size = sizeof(int32_t);
  pd->data.i32 = data;
  return pd;
}

pdata *pdata_from_u32(const uint32_t data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_U32;
  pd->size = sizeof(uint32_t);
  pd->data.u32 = data;
  return pd;
}

pdata *pdata_from_i64(const uint64_t data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_U64;
  pd->size = sizeof(int64_t);
  pd->data.i64 = data;
  return pd;
}

pdata *pdata_from_u64(const uint64_t data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_U64;
  pd->size = sizeof(uint64_t);
  pd->data.u64 = data;
  return pd;
}

pdata *pdata_from_float(const float data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_FLOAT;
  pd->size = sizeof(float);
  pd->data.f = data;
  return pd;
}

pdata *pdata_from_double(const double data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_DOUBLE;
  pd->size = sizeof(double);
  pd->data.d = data;
  return pd;
}

pdata *pdata_from_string(const char *data, const int move) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_STR;
  pd->size = strlen(data) + 1;
  pd->own_data = move;
  pd->data.p = (void *)data;

  return pd;
}

pdata *pdata_from_bytes(void *data, const size_t size, const int move) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_STR;
  pd->size = size;
  pd->own_data = move;
  pd->data.p = data;

  return pd;
}

pdata *pdata_from_ref(void *data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_RAW;
  pd->size = sizeof(void *);
  pd->own_data = 0;
  pd->data.p = data;
  return pd;
}

pdata *pdata_from_ud(
    void *data, const size_t size, const int move, const pdata_ud_free udfree) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_USER_DEFINED;
  pd->size = size;
  pd->data.p = data;
  pd->own_data = move;
  pd->udfree = udfree;
  return pd;
}

void pdata_free(const pdata *pd) {
  if (pd && pd->own_data) {
    if (pd->type == PDATA_USER_DEFINED && pd->udfree) {
      pd->udfree(pd->data.p);
    } else {
      free(pd->data.p);
    }
  }
  free((void *)pd);
}

void pdata_print(const pdata *pd) {
  if (pd == NULL) {
    printf("null\n");
    return;
  }
  switch (pd->type) {
  case PDATA_I8:
    printf("i8: %d\n", pd->data.i8);
    break;
  case PDATA_U8:
    printf("u8: %u\n", pd->data.u8);
    break;
  case PDATA_I16:
    printf("i16: %d\n", pd->data.i16);
  case PDATA_U16:
    printf("u16: %u\n", pd->data.u16);
    break;
  case PDATA_I32:
    printf("i32: %i\n", pd->data.i32);
    break;
  case PDATA_U32:
    printf("u32: %u\n", pd->data.u32);
    break;
  case PDATA_I64:
    printf("i64: %lld\n", pd->data.i64);
    break;
  case PDATA_U64:
    printf("u64: %llu\n", pd->data.u64);
    break;
  case PDATA_FLOAT:
    printf("float: %f\n", pd->data.f);
    break;
  case PDATA_DOUBLE:
    printf("double: %f\n", pd->data.d);
    break;
  case PDATA_STR:
    printf("str: %s\n", (char *)pd->data.p);
    break;
  case PDATA_RAW:
    printf("raw: %p, size: %lu\n", pd->data.p, pd->size);
    break;
  case PDATA_USER_DEFINED:
    printf(
        "ud: %p, size: %lu\n, udfree: %p\n", pd->data.p, pd->size, pd->udfree);
  default:
    printf("unknown type\n");
  }
}

void pdata_test() {
  const pdata *pi8 = pdata_from_i8(8);
  const pdata *pu8 = pdata_from_u8(8);
  const pdata *pu16 = pdata_from_u16(16);
  const pdata *pu32 = pdata_from_u32(32);
  const pdata *pu64 = pdata_from_u64(64);
  const pdata *pf = pdata_from_float(3.14);
  const pdata *pd = pdata_from_double(3.1415926);
  const pdata *ps = pdata_from_string("this is a string", 0);

  assert(pdata_i8(pi8) == 8);
  assert(pdata_u8(pu8) == 8);
  assert(pdata_u16(pu16) == 16);
  assert(pdata_u32(pu32) == 32);
  assert(pdata_u64(pu64) == 64);
  assert(pdata_float(pf) - 3.14 < FLT_EPSILON);
  assert(pdata_double(pd) - 3.1415926 < DBL_EPSILON);
  assert(strcmp(pdata_raw(ps), "this is a string") == 0);

  pdata_free(pi8);
  pdata_free(pu8);
  pdata_free(pu16);
  pdata_free(pu32);
  pdata_free(pu64);
  pdata_free(pf);
  pdata_free(pd);
  pdata_free(ps);
}
