/*
 *  main.c
 *  evaluator
 *
 *  Created by Master.G on 13-7-25.
 *  Copyright (c) 2013 MED. All rights reserved.
 */

#include <stdio.h>
#include <string.h>

#include "arraygenerator/utils.h"
#include "evalarrays.h"
#include "evaluator.h"

#define FLUSHES_COUNT 7169
#define UNIQUE3_COUNT FLUSHES_COUNT
#define OTHERS_COUNT 68922

void verify_arrays(void) {
  int flushes = 0;
  int unique3 = 0;
#ifndef EVAL_USE_HASH
  int others = 0;
#endif
  int i = 0;

  for (i = 0; i < FLUSHES_COUNT; i++) {
    if (eval_flushes[i] != 0)
      flushes++;
  }

  printf("there are %d entropy in Flushes\n", flushes);

  for (i = 0; i < UNIQUE3_COUNT; i++) {
    if (eval_unique3[i] != 0)
      unique3++;
  }

  printf("there are %d entropy in Unique3\n", unique3);

#ifndef EVAL_USE_HASH
  for (i = 0; i < OTHERS_COUNT; i++) {
    if (eval_others[i] != 0)
      others++;
  }

  printf("there are %d entropy in Others\n", others);
#endif
}

int main(int argc, const char *argv[]) {
  int i = 0;
  int entropy;
  int c1, c2, c3;
  int comb[52];
  int deck[52];

  int rank = 0;
  int ranks[7];

  int highest = 0;
  int lowest = LEOPARD_KILLER_ENTROPY;

  int64_t timestamp;

  memset(ranks, 0, sizeof(int) * 7);

  eval_init_deck(deck);

  for (i = 0; i < 52; i++) {
    comb[i] = i;
  }

  timestamp = GetTimeInMicroseconds();

  do {
    c1 = deck[comb[0]];
    c2 = deck[comb[1]];
    c3 = deck[comb[2]];
    entropy = eval_3hand(c1, c2, c3);

    if (entropy > highest) {
      highest = entropy;
    }

    if (entropy < lowest) {
      lowest = entropy;
    }

    printf("entropy %3d ", entropy);
    rank = eval_hand_rank(entropy);

    ranks[rank]++;

    print_card(c1);
    printf(" ");
    print_card(c2);
    printf(" ");
    print_card(c3);
    printf(" ");
    printf("%s \n", eval_hand_rank_description(rank));

  } while (next_comb(comb, 3, 52));

  timestamp = GetTimeInMicroseconds() - timestamp;
  printf("All combination evaluated in %lld microseconds\n", timestamp);

  printf("highest : %d ", highest);
  printf("%s \n", eval_hand_rank_description(eval_hand_rank(highest)));
  printf("lowest : %d ", lowest);
  printf("%s \n", eval_hand_rank_description(eval_hand_rank(lowest)));

  for (i = 1; i < 7; i++) {
    printf("%3d %s\n", ranks[i], eval_hand_rank_description(i));
  }

  verify_arrays();

  return 0;
}