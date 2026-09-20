//
// Created by mg on 2023/11/17.
//

#include <stdio.h>
#include <string.h>
#include <texas_array.h>
#include <texas_eval.h>
#include <texas_utils.h>

unsigned int make_card(const unsigned int rank, const unsigned int suit) {
  return texas_eval_primes[rank] | (rank << 8u) | suit | (1u << (16 + rank));
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf(
        "usage: %s <card>...  (5 or 7 cards, e.g. AS KS QS JS 10S)\n", argv[0]);
    return -1;
  }

  unsigned int cards[7];
  size_t card_index = 0;
  const int count = argc - 1;
  if (count != 5 && count != 7) {
    printf("invalid card count: %d (want 5 or 7)\n", count);
    return -1;
  }
  for (int i = 1; i < argc; i++) {
    unsigned int suit = 0;
    unsigned int rank = 0;
    size_t next_pos = 1;
    if (argv[i][0] == 'A') {
      rank = TEXAS_RANK_ACE;
    } else if (argv[i][0] == '2') {
      rank = 0;
    } else if (argv[i][0] == '3') {
      rank = 1;
    } else if (argv[i][0] == '4') {
      rank = 2;
    } else if (argv[i][0] == '5') {
      rank = 3;
    } else if (argv[i][0] == '6') {
      rank = 4;
    } else if (argv[i][0] == '7') {
      rank = 5;
    } else if (argv[i][0] == '8') {
      rank = 6;
    } else if (argv[i][0] == '9') {
      rank = 7;
    } else if (argv[i][0] == '1') {
      rank = 8;
      next_pos = 2;
    } else if (argv[i][0] == 'J') {
      rank = 9;
    } else if (argv[i][0] == 'Q') {
      rank = 10;
    } else if (argv[i][0] == 'K') {
      rank = 11;
    } else {
      printf("invalid rank: %c\n", argv[i][0]);
      return -1;
    }
    if (argv[i][next_pos] == 'C') {
      suit = TEXAS_CARD_SUIT_CLUB;
    } else if (argv[i][next_pos] == 'D') {
      suit = TEXAS_CARD_SUIT_DIAMOND;
    } else if (argv[i][next_pos] == 'H') {
      suit = TEXAS_CARD_SUIT_HEART;
    } else if (argv[i][next_pos] == 'S') {
      suit = TEXAS_CARD_SUIT_SPADE;
    } else {
      printf("unknown suit: %c\n", argv[i][next_pos]);
      return -1;
    }

    cards[card_index] = make_card(rank, suit);
    card_index++;
  }

  /* 7 张走完美哈希的快速路径, 与逐一枚举 21 个子集同结果 */
  const unsigned short value =
      count == 7
          ? texas_eval_7hand_fast(cards)
          : texas_eval_5hand(cards[0], cards[1], cards[2], cards[3], cards[4]);

  char desc[20];
  texas_utils_str_hand(texas_eval_hand_rank(value), desc);
  printf("eval value: %d (%s)\n", value, desc);

  return 0;
}
