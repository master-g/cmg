/*
The MIT License (MIT)

Copyright (c) 2014 Master.G

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "landlord.h"

#define BENCH_SEED_BEGIN 10000
#define BENCH_SEED_END 20000

/* benchmark: play every seed in the range and count who wins */
int main(void) {
  int peasantwon = 0;
  int landlordwon = 0;
  int illegal = 0; /* games stopped by a hand the rules reject */
  uint32_t seed;
  game_t game;

  printf("start at %ld\n", (long)time(NULL));

  Game_Init(&game);

  for (seed = BENCH_SEED_BEGIN; seed < BENCH_SEED_END; seed++) {
    Game_Play(&game, seed);

    if (game.status != GameStatus_Over)
      illegal++;
    else if (game.winner == game.landlord)
      landlordwon++;
    else
      peasantwon++;
  }

  printf("peasants : %d\n", peasantwon);
  printf("landlord : %d\n", landlordwon);
  printf("illegal  : %d\n", illegal);

  printf("ended at %ld\n", (long)time(NULL));
  printf("\n");

  return 0;
}
