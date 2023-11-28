/**
 * Created by MasterG on 2020/5/18.
 */

#include "texas_eval.h"
#include "texas_array.h"

static int c75_premult[21][5] = {
    {0, 1, 2, 3, 4}, {0, 1, 2, 3, 5}, {0, 1, 2, 3, 6}, {0, 1, 2, 4, 5},
    {0, 1, 2, 4, 6}, {0, 1, 2, 5, 6}, {0, 1, 3, 4, 5}, {0, 1, 3, 4, 6},
    {0, 1, 3, 5, 6}, {0, 1, 4, 5, 6}, {0, 2, 3, 4, 5}, {0, 2, 3, 4, 6},
    {0, 2, 3, 5, 6}, {0, 2, 4, 5, 6}, {0, 3, 4, 5, 6}, {1, 2, 3, 4, 5},
    {1, 2, 3, 4, 6}, {1, 2, 3, 5, 6}, {1, 2, 4, 5, 6}, {1, 3, 4, 5, 6},
    {2, 3, 4, 5, 6}};

unsigned int texas_eval_mph_search(unsigned int product) {
  unsigned int a, b, r;
  product += 0xe91aaa35;
  product ^= product >> 16u;
  product += product << 8u;
  product ^= product >> 4u;
  b = (product >> 8u) & 0x1ffu;
  a = (product + (product << 2u)) >> 19u;
  r = a ^ texas_eval_hash_tab[b];
  return r;
}

int texas_eval_hand_rank(unsigned short value) {
  if (value > 6185)
    return (TEXAS_HAND_HIGH_CARD);
  if (value > 3325)
    return (TEXAS_HAND_ONE_PAIR);
  if (value > 2467)
    return (TEXAS_HAND_TWO_PAIR);
  if (value > 1609)
    return (TEXAS_HAND_THREE_OF_A_KIND);
  if (value > 1599)
    return (TEXAS_HAND_STRAIGHT);
  if (value > 322)
    return (TEXAS_HAND_FLUSH);
  if (value > 166)
    return (TEXAS_HAND_FULL_HOUSE);
  if (value > 10)
    return (TEXAS_HAND_FOUR_OF_A_KIND);
  return (TEXAS_HAND_STRAIGHT_FLUSH);
}

int texas_eval_is_loyal_straight_flush(unsigned short value) {
  return value == 1 ? 1 : 0;
}

unsigned short texas_eval_5hand(
    unsigned int c1, unsigned int c2, unsigned int c3, unsigned int c4,
    unsigned int c5) {
  unsigned int q = (c1 | c2 | c3 | c4 | c5) >> 16u;
  unsigned short s;
  if (c1 & c2 & c3 & c4 & c5 & 0xF000u) {
    return texas_eval_flushes[q];
  }
  if ((s = texas_eval_unique5[q])) {
    return s;
  }

  return texas_eval_hash_values[texas_eval_mph_search(
      (c1 & 0xFFu) * (c2 & 0xFFu) * (c3 & 0xFFu) * (c4 & 0xFFu) *
      (c5 & 0xFFu))];
}

unsigned short texas_eval_7hand(
    unsigned int c1, unsigned int c2, unsigned int c3, unsigned int c4,
    unsigned int c5, unsigned int c6, unsigned int c7) {
  unsigned int c[7];
  int i, q, best = 9999;
  c[0] = c1;
  c[1] = c2;
  c[2] = c3;
  c[3] = c4;
  c[4] = c5;
  c[5] = c6;
  c[6] = c7;
  for (i = 0; i < 21; i++) {
    q = texas_eval_5hand(
        c[c75_premult[i][0]], c[c75_premult[i][1]], c[c75_premult[i][2]],
        c[c75_premult[i][3]], c[c75_premult[i][4]]);
    if (q < best) {
      best = q;
    }
  }
  return best;
}
