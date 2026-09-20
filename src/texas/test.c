//
// Created by MasterG on 2020/5/13.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bits.h"
#include "generate.h"
#include "texas_array.h"
#include "texas_array7.h"
#include "texas_eval.h"
#include "texas_utils.h"

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
    printf(
        "their size:%d != mine size:%d\n", size_eval_flushes,
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
    printf(
        "their size:%d != mine size:%d\n", size_eval_unique5,
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
      printf(
          "their[%d]=%d, mine[%d]=%d\n", iter->product, their, iter->product,
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

/* answer.c 是参考实现, texas_array.c 才是运行时真正查的表, 两者都要对拍 */
void test_runtime_arrays() {
  uint16_t flushes[MAGIC_UNIQUE5_SIZE] = {0};
  uint16_t unique5[MAGIC_UNIQUE5_SIZE] = {0};
  texas_magic_kv_t *others = NULL;
  texas_magic_kv_t *iter = NULL;

  printf("testing runtime arrays...\n");

  texas_gen_flushes(flushes);
  texas_gen_unique5(unique5);

  for (int i = 0; i < MAGIC_UNIQUE5_SIZE; i++) {
    if (texas_eval_flushes[i] != flushes[i]) {
      printf(
          "flushes[%d]: runtime=%d, mine=%d\n", i, texas_eval_flushes[i],
          flushes[i]);
      exit(-1);
    }
    if (texas_eval_unique5[i] != unique5[i]) {
      printf(
          "unique5[%d]: runtime=%d, mine=%d\n", i, texas_eval_unique5[i],
          unique5[i]);
      exit(-1);
    }
  }

  /* 走完整的运行时路径: 素数积 -> 完美哈希 -> 取值 */
  texas_gen_others(&others);
  int ret = 0;
  iter = others;
  while (iter != NULL) {
    uint16_t runtime =
        texas_eval_hash_values[texas_eval_mph_search(iter->product)];
    if (runtime != iter->magic) {
      printf(
          "others[%d]: runtime=%d, mine=%d\n", iter->product, runtime,
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

/* 逐个枚举 49205 个点数多重集, 校验 7 张快速表与参考实现一致 */
static unsigned int g_pow5[13];
static unsigned int g_counts[13];
static long g_checked, g_bad;

static void check_multiset(void) {
  unsigned int c[7], q = 0;
  int sc[4] = {0, 0, 0, 0};
  int n = 0;
  for (int r = 0; r < 13; r++) {
    for (unsigned int k = 0; k < g_counts[r]; k++) {
      int best = -1;
      for (int s = 0; s < 4; s++) {
        int used = 0;
        for (int t = 0; t < (int)k; t++)
          if (((c[n - 1 - t] >> 12u) & 0xFu) == (1u << s))
            used = 1;
        if (!used && (best < 0 || sc[s] < sc[best]))
          best = s;
      }
      c[n++] = texas_eval_primes[r] | ((unsigned)r << 8u) | (0x1000u << best) |
               (1u << (16 + r));
      sc[best]++;
    }
  }
  for (int i = 0; i < 13; i++)
    q += g_counts[i] * g_pow5[i];

  unsigned short want =
      texas_eval_7hand(c[0], c[1], c[2], c[3], c[4], c[5], c[6]);
  unsigned short got = texas_eval7_values[texas_eval7_mph(q)];
  if (got != want) {
    if (++g_bad <= 5)
      printf("multiset key %u: table=%d, reference=%d\n", q, got, want);
  }
  /* 同时走一遍完整的快速入口 */
  if (texas_eval_7hand_fast(c) != want) {
    if (++g_bad <= 5)
      printf("fast path key %u differs\n", q);
  }
  g_checked++;
}

static void walk_multisets(int rank, int left) {
  if (rank == 13) {
    if (left == 0)
      check_multiset();
    return;
  }
  for (unsigned int k = 0; k <= 4 && (int)k <= left; k++) {
    g_counts[rank] = k;
    walk_multisets(rank + 1, left - (int)k);
  }
  g_counts[rank] = 0;
}

void test_7card_table() {
  printf("testing 7-card table...\n");
  g_pow5[0] = 1;
  for (int i = 1; i < 13; i++)
    g_pow5[i] = g_pow5[i - 1] * 5;
  walk_multisets(0, 7);
  if (g_checked != 49205) {
    printf("expected 49205 multisets, got %ld\n", g_checked);
    exit(-1);
  }
  if (g_bad)
    exit(-1);

  /* 同花路径: 枚举每种花色张数 >= 5 的情形太多, 抽查固定种子的随机样本 */
  unsigned int deck[52];
  texas_utils_init_deck(deck);
  unsigned int seed = 20260920u;
  long flush_checked = 0;
  for (long it = 0; it < 400000; it++) {
    unsigned int c[7];
    int idx[7];
    for (int i = 0; i < 7; i++) {
      int ok;
      do {
        ok = 1;
        seed = seed * 1103515245u + 12345u;
        idx[i] = (int)((seed >> 16u) % 52u);
        for (int j = 0; j < i; j++)
          if (idx[j] == idx[i])
            ok = 0;
      } while (!ok);
      c[i] = deck[idx[i]];
    }
    unsigned short want =
        texas_eval_7hand(c[0], c[1], c[2], c[3], c[4], c[5], c[6]);
    if (texas_eval_7hand_fast(c) != want) {
      printf("random 7-card hand differs: %d\n", want);
      exit(-1);
    }
    if (texas_eval_hand_rank(want) <= TEXAS_HAND_FLUSH)
      flush_checked++;
  }
  printf(
      "  %ld multisets, 400000 random hands (%ld flush-or-better)\n", g_checked,
      flush_checked);
}

int main(int argc, char *argv[]) {
  test_flushes();
  test_unique5();
  test_others();
  test_runtime_arrays();
  test_7card_table();

  return 0;
}
