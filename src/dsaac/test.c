#include <assert.h>
#include <float.h>

#include "list.h"
#include "payload.h"
#include "stack.h"
#include "test.h"

void custom_free(void *data) { free(data); }

void test_payload() {
  const pdata *pi8 = pdata_from_i8(8);
  const pdata *pu8 = pdata_from_u8(8);
  const pdata *pu16 = pdata_from_u16(16);
  const pdata *pu32 = pdata_from_u32(32);
  const pdata *pu64 = pdata_from_u64(64);
  const pdata *pf = pdata_from_float(3.14);
  const pdata *pd = pdata_from_double(3.1415926);
  const pdata *ps = pdata_from_str("this is a string");

  assert(pdata_i8(pi8) == 8);
  assert(pdata_u8(pu8) == 8);
  assert(pdata_u16(pu16) == 16);
  assert(pdata_u32(pu32) == 32);
  assert(pdata_u64(pu64) == 64);
  assert(pdata_float(pf) - 3.14 < FLT_EPSILON);
  assert(pdata_double(pd) - 3.1415926 < DBL_EPSILON);
  assert(strcmp(pdata_raw(ps), "this is a string") == 0);

  const char *ud = (char *)malloc(1024);
  const pdata *pud = pdata_from_ud((void *)ud, 1024, (void *)custom_free);

  pdata_free(pi8);
  pdata_free(pu8);
  pdata_free(pu16);
  pdata_free(pu32);
  pdata_free(pu64);
  pdata_free(pf);
  pdata_free(pd);
  pdata_free(ps);
  pdata_free(pud);
}

void test_list() {
  list_t *l = list_alloc();

  // generate test cases for list
  const pdata *pi8 = pdata_from_i8(8);
  const pdata *pu8 = pdata_from_u8(8);
  const pdata *pu16 = pdata_from_u16(16);
  const pdata *pu32 = pdata_from_u32(32);
  const pdata *pu64 = pdata_from_u64(64);
  const pdata *pf = pdata_from_float(3.14);
  const pdata *pd = pdata_from_double(3.1415926);
  const pdata *ps = pdata_from_str("this is a string");
  const char *ud = (char *)malloc(1024);
  const pdata *pud = pdata_from_ud((void *)ud, 1024, (void *)custom_free);

  list_unshift(l, pi8);
  list_push(l, pu8);
  list_insert(l, 1, pu16);
  list_insert(l, 2, pu32);
  list_push(l, pu64);
  list_unshift(l, pf);
  list_insert(l, 1, pd);
  list_insert(l, 2, ps);
  list_push(l, pud);

  assert(list_first(l) == pf);

  const int idx = list_index_of(l, pud);
  assert(idx == 8);
  const pdata *fpud = list_remove_at(l, idx);
  assert(fpud == pud);
  pdata_free(fpud);

  list_foreach(l, pdata_free);

  list_free(l);
}

void test_stack() {
  stack_t *s = stack_alloc();

  const pdata *pi8 = pdata_from_i8(8);
  const pdata *pu8 = pdata_from_u8(8);
  const pdata *pu16 = pdata_from_u16(16);
  const pdata *pu32 = pdata_from_u32(32);
  const pdata *pu64 = pdata_from_u64(64);
  const pdata *pf = pdata_from_float(3.14);
  const pdata *pd = pdata_from_double(3.1415926);
  const pdata *ps = pdata_from_str("this is a string");
  const char *ud = (char *)malloc(1024);
  const pdata *pud = pdata_from_ud((void *)ud, 1024, (void *)custom_free);

  stack_push(s, pi8);
  stack_push(s, pu8);
  stack_push(s, pu16);
  stack_push(s, pu32);
  stack_push(s, pu64);
  stack_push(s, pf);
  stack_push(s, pd);
  stack_push(s, ps);
  stack_push(s, pud);

  while (!stack_is_empty(s)) {
    const pdata *d = stack_pop(s);
    pdata_free(d);
  }

  stack_free(s);
}

void test_queue() {}

void test_tree() {}
