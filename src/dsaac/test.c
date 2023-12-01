#include <assert.h>
#include <float.h>

#include "list.h"
#include "payload.h"
#include "queue.h"
#include "stack.h"
#include "test.h"
#include "tree.h"

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

void test_list_iter() {
  list_t *l = list_alloc();
  list_push(l, pdata_from_i8(0));
  list_push(l, pdata_from_i8(1));
  list_push(l, pdata_from_i8(2));
  list_push(l, pdata_from_i8(3));
  list_push(l, pdata_from_i8(4));

  printf("[");
  list_iter_t *iter = list_iter_alloc(l);
  const pdata *d = NULL;
  while ((d = list_iter_next(iter)) != NULL) {
    printf("%d, ", pdata_i8(d));
  }
  list_iter_free(iter);
  printf("]\n");

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
    char *buf = NULL;
    size_t size = 0;
    const pdata *d = stack_pop(s);
    pdata_print(d, &buf, &size);
    free(buf);
    pdata_free(d);
  }

  stack_free(s);
}

void test_queue() {
  queue_t *q = queue_alloc();

  queue_enqueue(q, pdata_from_i8(8));
  queue_enqueue(q, pdata_from_u8(8));
  queue_enqueue(q, pdata_from_u16(16));
  queue_enqueue(q, pdata_from_u32(32));
  queue_enqueue(q, pdata_from_u64(64));
  queue_enqueue(q, pdata_from_float(3.14));
  queue_enqueue(q, pdata_from_double(3.1415926));
  queue_enqueue(q, pdata_from_str("this is a string"));
  const char *ud = (char *)malloc(1024);
  queue_enqueue(q, pdata_from_ud((void *)ud, 1024, (void *)custom_free));

  while (!queue_is_empty(q)) {
    char *buf = NULL;
    size_t size = 0;
    const pdata *d = queue_dequeue(q);
    pdata_print(d, &buf, &size);
    free(buf);
    pdata_free(d);
  }

  queue_free(q);
}

void free_tree_payload(const tree_t *t) {
  if (t && t->data) {
    pdata_free(t->data);
  }
}

void test_tree() {
  tree_t *root = tree_alloc(pdata_from_str("root"));
  tree_t *n1_1 = tree_alloc(pdata_from_str("n1_1"));
  tree_t *n1_2 = tree_alloc(pdata_from_str("n1_2"));
  tree_t *n2_1 = tree_alloc(pdata_from_str("n2_1"));
  tree_t *n2_2 = tree_alloc(pdata_from_str("n2_2"));
  tree_t *n2_3 = tree_alloc(pdata_from_str("n2_3"));
  tree_t *n3_1 = tree_alloc(pdata_from_str("n3_1"));

  root->child = n1_1;
  n1_1->parent = root;
  n1_1->sibling = n1_2;
  n1_2->parent = root;
  n1_2->sibling = NULL;
  n1_2->child = n2_1;
  n2_1->parent = n1_2;
  n2_1->sibling = n2_2;
  n2_2->parent = n1_2;
  n2_2->sibling = n2_3;
  n2_3->parent = n1_2;
  n2_3->sibling = NULL;
  n2_3->child = n3_1;
  n3_1->parent = n2_3;

  char *buf = NULL;
  size_t size = 0;
  tree_to_dot(root, &buf, &size);
  printf("%s\n", buf);
  free(buf);

  tree_foreach(root, free_tree_payload);
  tree_free(root);
}
