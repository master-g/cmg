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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BENCH_SEED_BEGIN 10000
#define BENCH_SEED_END 20000

/* play one seed with one AI as landlord and another as both peasants */
static bool landlord_wins(
    game_t *game, uint32_t seed, int landlord, const ai_t *as_landlord,
    const ai_t *as_peasants) {
  int i;

  for (i = 0; i < GAME_PLAYERS; i++)
    game->players[i].ai = i == landlord ? as_landlord : as_peasants;

  game_play(game, seed);

  /* every AI bids alike, so who sits where never changes the landlord */
  if ((game->status != GAME_STATUS_OVER) || (game->landlord != landlord)) {
    fprintf(stderr, "seed %u: duel went wrong\n", (unsigned)seed);
    exit(1);
  }

  return game->winner == game->landlord;
}

/*
 * Two AIs at one table. Every seed is played twice with the same cards: a
 * as landlord against two b, then b as landlord against two a. An AI playing
 * itself tells nothing, both sides get better or worse together.
 */
static void
duel(const char *name_a, const ai_t *a, const char *name_b, const ai_t *b) {
  int a_landlord = 0; /* games a won as landlord */
  int b_landlord = 0;
  int games = BENCH_SEED_END - BENCH_SEED_BEGIN;
  uint32_t seed;
  game_t game;

  game_init(&game);

  for (seed = BENCH_SEED_BEGIN; seed < BENCH_SEED_END; seed++) {
    int landlord;

    game_play(&game, seed);
    landlord = game.landlord;

    a_landlord += landlord_wins(&game, seed, landlord, a, b);
    b_landlord += landlord_wins(&game, seed, landlord, b, a);
  }

  printf(
      "%s vs %s, %d seeds, each played from both sides\n", name_a, name_b,
      games);
  printf(
      "  %-8s as landlord: %5d  as peasants: %5d  total: %.1f%%\n", name_a,
      a_landlord, games - b_landlord,
      100.0 * (a_landlord + games - b_landlord) / (2 * games));
  printf(
      "  %-8s as landlord: %5d  as peasants: %5d  total: %.1f%%\n", name_b,
      b_landlord, games - a_landlord,
      100.0 * (b_landlord + games - a_landlord) / (2 * games));
}

/* benchmark: play every seed in the range and count who wins */
int main(void) {
  int peasantwon = 0;
  int landlordwon = 0;
  int illegal = 0; /* games stopped by a hand the rules reject */
  uint32_t seed;
  game_t game;

  printf("start at %ld\n", (long)time(NULL));

  game_init(&game);

  for (seed = BENCH_SEED_BEGIN; seed < BENCH_SEED_END; seed++) {
    game_play(&game, seed);

    if (game.status != GAME_STATUS_OVER)
      illegal++;
    else if (game.winner == game.landlord)
      landlordwon++;
    else
      peasantwon++;
  }

  printf("peasants : %d\n", peasantwon);
  printf("landlord : %d\n", landlordwon);
  printf("illegal  : %d\n", illegal);

  printf("\n");

  duel("counted", &ai_counted, "advanced", &ai_advanced);
  duel("counted", &ai_counted, "standard", &ai_standard);
  duel("advanced", &ai_advanced, "standard", &ai_standard);

  printf("ended at %ld\n", (long)time(NULL));
  printf("\n");

  return 0;
}
