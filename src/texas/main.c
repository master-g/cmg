//
// Created by MasterG on 2020/5/13.
//

#include <stdio.h>

#include "generate.h"

int main(int argc, char* argv[]) {
  uint32_t deck[52] = {0};
  texas_init_deck(deck);

  for (int i = 0; i < 52; i++) {
    printf("%d\n", deck[i]);
  }

  return 0;
}

