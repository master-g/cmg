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

typedef struct pdata pdata;

typedef void (*pdata_ud_free)(const void *ud);

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
pdata *pdata_from_str(const char *data);
pdata *pdata_from_ref(void *data);
pdata *pdata_from_ud(void *data, size_t size, pdata_ud_free udfree);

int pdata_type(const pdata *data);
int8_t pdata_i8(const pdata *data);
uint8_t pdata_u8(const pdata *data);
int16_t pdata_i16(const pdata *data);
uint16_t pdata_u16(const pdata *data);
int32_t pdata_i32(const pdata *data);
uint32_t pdata_u32(const pdata *data);
int64_t pdata_i64(const pdata *data);
uint64_t pdata_u64(const pdata *data);
float pdata_float(const pdata *data);
double pdata_double(const pdata *data);
char *pdata_str(const pdata *data);
void *pdata_raw(const pdata *data);

void pdata_free(const pdata *pd);

void pdata_print(const pdata *pd);

#endif /* PAYLOAD_H_ */
