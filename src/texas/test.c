//
// Created by MasterG on 2020/5/13.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bits.h"
#include "generate.h"

extern uint16_t eval_flushes[];
extern uint16_t eval_unique5[];
extern int size_eval_flushes;
extern int size_eval_unique5;
extern uint32_t eval_find_fast(uint32_t product);
extern uint16_t eval_others(int product);

void test_flushes() {
  uint16_t flushes[MAGIC_UNIQUE5_SIZE] = {0};
  texas_gen_flushes(flushes);

  printf("testing flushes...\n");
  if (size_eval_flushes != MAGIC_UNIQUE5_SIZE) {
    printf("their size:%d != mine size:%d\n", size_eval_flushes,
           MAGIC_UNIQUE5_SIZE);
    exit(-1);
  }

  for (int i = 0; i < size_eval_flushes; i++) {
    if (eval_flushes[i] != flushes[i]) {
      printf("their[%d]=%d, mine[%d]=%d\n", i, eval_flushes[i], i, flushes[i]);
      exit(-1);
    }
  }
}

void test_unique5() {
  uint16_t unique5[MAGIC_UNIQUE5_SIZE] = {0};
  texas_gen_unique5(unique5);

  printf("testing unique5...\n");
  if (size_eval_unique5 != MAGIC_UNIQUE5_SIZE) {
    printf("their size:%d != mine size:%d\n", size_eval_unique5,
           MAGIC_UNIQUE5_SIZE);
    exit(-1);
  }

  for (int i = 0; i < size_eval_unique5; i++) {
    if (eval_unique5[i] != unique5[i]) {
      printf("their[%d]=%d, mine[%d]=%d\n", i, eval_unique5[i], i, unique5[i]);
      exit(-1);
    }
  }
}

void test_others() {
  texas_magic_kv_t *others = NULL;
  texas_magic_kv_t *iter = NULL;
  texas_gen_others(&others);

  printf("testing others...\n");

  int ret = 0;
  iter = others;
  while (iter != NULL) {
    uint16_t their = eval_others(iter->product);
    if (their != iter->magic) {
      printf("their[%d]=%d, mine[%d]=%d\n", iter->product, their, iter->product,
             iter->magic);
      ret = -1;
      break;
    }
    iter = iter->next;
  }

  texas_free_others(others);
  if (ret != 0) {
    exit(ret);
  }
}

int main(int argc, char *argv[]) {
  test_flushes();
  test_unique5();
  test_others();

  return 0;
}
