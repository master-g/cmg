/*
 *  pd.c
 *  DSAAC
 *
 *  Created by Master.G on 13-8-18.
 *  Copyright (c) 2013 Master.G. All rights reserved.
 */

#include "payload.h"

#include <assert.h>

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

typedef union {
  int8_t i8;
  uint8_t u8;
  int16_t i16;
  uint16_t u16;
  int32_t i32;
  uint32_t u32;
  int64_t i64;
  uint64_t u64;
  float f;
  double d;
  void *p;
} pdata_t;

struct pdata {
  int type;
  size_t size;
  pdata_t data;
  pdata_ud_free udfree;
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

pdata *pdata_from_str(const char *data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_STR;
  pd->size = strlen(data) + 1;
  pd->data.p = (void *)data;

  return pd;
}

pdata *pdata_from_ref(void *data) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_RAW;
  pd->size = sizeof(void *);
  pd->data.p = data;
  return pd;
}

pdata *
pdata_from_ud(void *data, const size_t size, const pdata_ud_free udfree) {
  pdata *pd = malloc(sizeof(pdata));
  pd->type = PDATA_USER_DEFINED;
  pd->size = size;
  pd->data.p = data;
  pd->udfree = udfree;
  return pd;
}

int pdata_type(const pdata *data) {
  if (data == NULL) {
    return -1;
  }
  return data->type;
}

int8_t pdata_i8(const pdata *data) {
  assert(data->type == PDATA_I8);
  return data->data.i8;
}

uint8_t pdata_u8(const pdata *data) {
  assert(data->type == PDATA_U8);
  return data->data.u8;
}

int16_t pdata_i16(const pdata *data) {
  assert(data->type == PDATA_I16);
  return data->data.i16;
}

uint16_t pdata_u16(const pdata *data) {
  assert(data->type == PDATA_U16);
  return data->data.u16;
}

int32_t pdata_i32(const pdata *data) {
  assert(data->type == PDATA_I32);
  return data->data.i32;
}

uint32_t pdata_u32(const pdata *data) {
  assert(data->type == PDATA_U32);
  return data->data.u32;
}

int64_t pdata_i64(const pdata *data) {
  assert(data->type == PDATA_I64);
  return data->data.i64;
}

uint64_t pdata_u64(const pdata *data) {
  assert(data->type == PDATA_U64);
  return data->data.u64;
}

float pdata_float(const pdata *data) {
  assert(data->type == PDATA_FLOAT);
  return data->data.f;
}

double pdata_double(const pdata *data) {
  assert(data->type == PDATA_DOUBLE);
  return data->data.d;
}

char *pdata_str(const pdata *data) {
  assert(data->type == PDATA_STR);
  return data->data.p;
}

void *pdata_raw(const pdata *data) { return data->data.p; }

void pdata_free(const pdata *pd) {
  if (pd && pd->type == PDATA_USER_DEFINED && pd->udfree) {
    pd->udfree(pd->data.p);
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
    printf("raw: %p\n", pd->data.p);
    break;
  case PDATA_USER_DEFINED:
    printf(
        "ud: %p, size: %lu\n, udfree: %p\n", pd->data.p, pd->size, pd->udfree);
  default:
    printf("unknown type\n");
  }
}
