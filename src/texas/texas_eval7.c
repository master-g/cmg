/**
 * Created by mg on 2026/9/20.
 *
 * 7 张牌的快速求值, 查 texas_generate7 生成的表, 与 texas_eval_7hand 同结果。
 */

#include "texas_array.h"
#include "texas_array7.h"
#include "texas_eval.h"

/* 10 个同花顺位型, 由高到低: AKQJT ... 65432, 5432A */
static const unsigned short texas_eval7_straights[10] = {
    0x1F00, 0x0F80, 0x07C0, 0x03E0, 0x01F0,
    0x00F8, 0x007C, 0x003E, 0x001F, 0x100F};

unsigned int texas_eval7_mph(unsigned int v) {
  v += TEXAS_EVAL7_SALT;
  v ^= v >> 16u;
  v += v << 8u;
  v ^= v >> 4u;
  return ((v + (v << TEXAS_EVAL7_SHIFT_A1)) >> TEXAS_EVAL7_SHIFT_A2) ^
         texas_eval7_hash_tab[(v >> TEXAS_EVAL7_SHIFT_B) & TEXAS_EVAL7_MASK_B];
}

unsigned short texas_eval_7hand_fast(const unsigned int *cards) {
  unsigned long long acc = 0;
  unsigned int suits, hit, v;
  int i;

  /* 一次加法同时累计点数五进制码(低 32 位)和花色张数(高 32 位, 每花色一字节) */
  for (i = 0; i < 7; i++)
    acc += texas_eval7_card[(cards[i] >> 8u) & 0xFFu];

  suits = (unsigned int)(acc >> 32u);
  hit = (suits + 0x03030303u) &
        0x08080808u; /* 某花色 >= 5 张则该字节进位到 bit3 */

  if (hit) {
    /* 7 张里某花色满 5 张时, 葫芦和四条在组合上不可能, 答案必是同花或同花顺 */
    /* clang/gcc 内建; bits.c 里的循环版在这条路径上太慢 */
    unsigned int bit = 0x1000u << ((unsigned)__builtin_ctz(hit) >> 3u);
    unsigned int fm = 0;
    for (i = 0; i < 7; i++)
      if (cards[i] & bit)
        fm |= cards[i] >> 16u;
    for (i = 0; i < 10; i++)
      if ((fm & texas_eval7_straights[i]) == texas_eval7_straights[i])
        return texas_eval_flushes[texas_eval7_straights[i]];
    while (__builtin_popcount(fm) > 5)
      fm &= fm - 1; /* 剥掉最低位, 留最大 5 张 */
    return texas_eval_flushes[fm];
  }

  v = (unsigned int)acc;
  return texas_eval7_values[texas_eval7_mph(v)];
}
