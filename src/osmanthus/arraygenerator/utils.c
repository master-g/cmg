/*
 *  utils.c
 *  arraygenerator
 *
 *  Created by Master.G on 13-7-25.
 *  Copyright (c) 2013 SNSTEAM. All rights reserved.
 */

#include "texas_utils.h"
#include "constants.h"
#include "evaluator/evalarrays.h"

char szDIAMOND[] = {0xE2, 0x99, 0xA6, 0};
char szCLUB[] = {0xE2, 0x99, 0xA3, 0};
char szHEART[] = {0xE2, 0x99, 0xA5, 0};
char szSPADE[] = {0xE2, 0x99, 0xA0, 0};

static struct timeval gsInitial;
static int gsInitializedTime = 0;

int64_t GetTimeInMicroseconds() {
  if (!gsInitializedTime) {
    gettimeofday(&gsInitial, 0);
    gsInitializedTime = 1;
  }

  struct timeval currentTime;
  gettimeofday(&currentTime, 0);

  struct timeval deltaTime;
  timersub(&currentTime, &gsInitial, &deltaTime);

  return 1000000 * deltaTime.tv_sec + deltaTime.tv_usec;
}

/*
 next_comb(int comb[], int k, int n)
 Generates the next combination of n elements as k after comb

 comb => the previous combination ( use (0, 1, 2, ..., k) for first)
 k => the size of the subsets to generate
 n => the size of the original set

 Returns: 1 if a valid combination was found
 0, otherwise
 */
int next_comb(int comb[], int k, int n) {
  int i = k - 1;
  ++comb[i];
  while ((i > 0) && (comb[i] >= n - k + 1 + i)) {
    --i;
    /* printf(">>>>>>>>> %d\n", i); */
    ++comb[i];
  }

  if (comb[0] > n - k) /* Combination (n-k, n-k+1, ..., n) reached */
    return 0;          /* No more combinations can be generated */

  /* comb now looks like (..., x, n, n, n, ..., n).
     Turn it into (..., x, x + 1, x + 2, ...) */
  for (i = i + 1; i < k; ++i)
    comb[i] = comb[i - 1] + 1;

  return 1;
}

void print_rank(int card) {
  int rank;
  rank = RANK(card);

  switch (rank) {
  case Jack:
    printf("J");
    break;
  case Queen:
    printf("Q");
    break;
  case King:
    printf("K");
    break;
  case Ace:
    printf("A");
    break;
  default:
    printf("%d", rank + 2);
    break;
  }
}

void print_suit(int card) {
  int suit;
  suit = card & 0x0000F000;

  switch (suit) {
  case DIAMOND:
    printf("%s", szDIAMOND);
    break;
  case CLUB:
    printf("%s", szCLUB);
    break;
  case HEART:
    printf("%s", szHEART);
    break;
  case SPADE:
    printf("%s", szSPADE);
    break;
  default:
    break;
  }
}

void print_card(int card) {
  print_suit(card);
  print_rank(card);
}
