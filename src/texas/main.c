//
// Created by MasterG on 2020/5/13.
//

#include <stdio.h>
#include <string.h>

#include "bits.h"
#include "generate.h"

extern uint16_t eval_flushes[];
extern uint16_t eval_unique5[];
extern int size_eval_flushes;
extern int size_eval_unique5;
extern uint16_t eval_others(int product);

int examine_flushes(const uint16_t *t) {
  if (size_eval_flushes != MAGIC_UNIQUE5_SIZE) {
    printf("their size:%d != mine size:%d\n", size_eval_flushes,
           MAGIC_UNIQUE5_SIZE);
    return -1;
  }

  for (int i = 0; i < size_eval_flushes; i++) {
    if (eval_flushes[i] != t[i]) {
      printf("their[%d]=%d, mine[%d]=%d\n", i, eval_flushes[i], i, t[i]);
      return -1;
    }
  }

  return 0;
}

int examine_unique5(const uint16_t *t) {
  if (size_eval_unique5 != MAGIC_UNIQUE5_SIZE) {
    printf("their size:%d != mine size:%d\n", size_eval_unique5,
           MAGIC_UNIQUE5_SIZE);
    return -1;
  }

  for (int i = 0; i < size_eval_unique5; i++) {
    if (eval_unique5[i] != t[i]) {
      printf("their[%d]=%d, mine[%d]=%d\n", i, eval_unique5[i], i, t[i]);
      return -1;
    }
  }

  return 0;
}

void dump_array(const char *name, uint16_t *arr, int len) {
  const int max_columns = 12;
  char buf[120];
  int row_num = len / max_columns;
  int tail = len % max_columns;

  printf("unsigned short texas_eval_flushes[] = {\n");

  for (int row = 0; row <= row_num; row++) {
    memset(buf, 0, sizeof(buf));
    int columns;
    int is_tail;
    if (row == row_num) {
      columns = tail;
      is_tail = 1;
    } else {
      columns = max_columns;
      is_tail = 0;
    }

    for (int col = 0; col < columns; col++) {
      char blob[6];
      int offset = row * columns;
      int buf_off = 6 * col; // strlen("%4d, ") = 6
      uint16_t v = arr[col + offset];
      int n;
      if (is_tail) {
        n = snprintf(blob, sizeof(blob), "%d", v);
      } else {
        n = snprintf(blob, sizeof(blob), "%d,", v);
      }

      if (col == columns - 1) {
        memset(blob + n, '\0', 6 - n);
      } else {
        memset(blob + n, ' ', 6 - n);
      }
      snprintf(buf + buf_off, sizeof(buf), "%s", blob);
    }
    printf("    %s\n", buf);
  }

  printf("};\n");
}

int gen_flushes() {
  // generate arr_flushes
  uint16_t flushes[MAGIC_UNIQUE5_SIZE] = {0};
  texas_gen_flushes(flushes);

  // examine
  if (examine_flushes(flushes) != 0) {
    return -1;
  }

  printf("/**\n");
  printf(" * this is a table lookup for all \"flush\" hands (e.g.  both\n");
  printf(" * flushes and straight-flushes).  entries containing a zero\n");
  printf(" * mean that combination is not possible with a five-card\n");
  printf(" * flush hand.\n");
  printf(" */\n");

  dump_array("texas_eval_flushes", flushes, MAGIC_UNIQUE5_SIZE);

  return 0;
}

int gen_unique5() {
  // generate arr_unique5
  uint16_t unique5[MAGIC_UNIQUE5_SIZE] = {0};
  texas_gen_unique5(unique5);

  // examine
  if (examine_unique5(unique5) != 0) {
    return -1;
  }

  printf("/**\n");
  printf(" * this is a table lookup for all non-flush hands consisting\n");
  printf(" * of five unique ranks (i.e.  either Straights or High Card\n");
  printf(" * hands).  it's similar to the above \"flushes\" array.\n");
  printf(" */\n");

  dump_array("texas_eval_unique5", unique5, MAGIC_UNIQUE5_SIZE);

  return 0;
}

void test_others() {
  texas_magic_kv_t *list = NULL;
  texas_gen_others(&list);
  texas_magic_kv_t *iter = list;
  while (iter != NULL) {
    printf("product:%d, magic:%d\n", iter->product, iter->magic);
    iter = iter->next;
  }
  texas_free_others(list);
}

void foo() {
  int index[2] = {0, 1};
  for (int i = 0; i < 13; i++) {
    for (int j = 0; j < 13; j++) {
      if (i != j) {
        printf("{%d, %d}\n", i, j);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int ret = 0;

  test_others();

  return ret;
}
