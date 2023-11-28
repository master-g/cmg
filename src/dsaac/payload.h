/*
 *  payload.h
 *  DSAAC
 *
 *  Created by Master.G on 13-8-18.
 *  Copyright (c) 2013 Master.G. All rights reserved.
 */

#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include "common.h"

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

typedef void (*pdata_ud_free)(const void *ud);

typedef struct pdata {
  int type;
  size_t size;
  int own_data;
  pdata_t data;
  pdata_ud_free udfree;

} pdata;

typedef int (*pdata_cmp_func)(const pdata *a, const pdata *b);

pdata *pdata_from_i8(uint8_t data);
pdata *pdata_from_u8(uint8_t data);
pdata *pdata_from_i16(uint16_t data);
pdata *pdata_from_u16(uint16_t data);
pdata *pdata_from_i32(uint32_t data);
pdata *pdata_from_u32(uint32_t data);
pdata *pdata_from_i64(uint64_t data);
pdata *pdata_from_u64(uint64_t data);
pdata *pdata_from_float(float data);
pdata *pdata_from_double(double data);
pdata *pdata_from_string(const char *data, int move);
pdata *pdata_from_bytes(void *data, size_t size, int move);
pdata *pdata_from_ud(void *data, size_t size, int move, pdata_ud_free udfree);
pdata *pdata_from_ref(void *data);

#define pdata_i8(pd) ((pd)->data.i8)
#define pdata_u8(pd) ((pd)->data.u8)
#define pdata_i16(pd) ((pd)->data.i16)
#define pdata_u16(pd) ((pd)->data.u16)
#define pdata_i32(pd) ((pd)->data.i32)
#define pdata_u32(pd) ((pd)->data.u32)
#define pdata_i64(pd) ((pd)->data.i64)
#define pdata_u64(pd) ((pd)->data.u64)
#define pdata_float(pd) ((pd)->data.f)
#define pdata_double(pd) ((pd)->data.d)
#define pdata_raw(pd) ((pd)->data.p)

void pdata_free(const pdata *pd);

void pdata_print(const pdata *pd);

void pdata_test();

#endif /* PAYLOAD_H_ */
