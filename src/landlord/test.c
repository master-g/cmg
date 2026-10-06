/*
 * landlord self-check: run by `make test`, exits non-zero on failure.
 *
 * Two seams are tested: the rules (cards -> hand type) and a whole game
 * (seed -> one finished game).
 */

#undef NDEBUG
#include <assert.h>

#include "landlord.h"

/* ************************************************************
 * rules: cards -> hand type
 * ************************************************************/

typedef struct {
  const char *cards;
  int type;
} parse_case_t;

static const parse_case_t parse_cases[] = {
    {"♣3", Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAINLESS)},
    {"♣3 ♠3", Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAINLESS)},
    {"♠r ♠R", Hand_Format(HAND_PRIMAL_NUKE, HAND_KICKER_NONE, HAND_CHAINLESS)},
    {"♠6 ♥6 ♦6",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAINLESS)},
    {"♣3 ♠3 ♥3 ♥7",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, HAND_CHAINLESS)},
    {"♣3 ♣5 ♠5 ♠3 ♥5",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, HAND_CHAINLESS)},
    {"♣7 ♠7 ♥7 ♦7",
     Hand_Format(HAND_PRIMAL_BOMB, HAND_KICKER_NONE, HAND_CHAINLESS)},
    {"♣3 ♣4 ♠5 ♠6 ♥7",
     Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAIN)},
    {"♣3 ♠4 ♦6 ♥8 ♠7 ♦5 ♦9 ♦T ♦J",
     Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAIN)},
    {"♣4 ♠4 ♠5 ♥6 ♥5 ♦6",
     Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAIN)},
    {"♣4 ♠4 ♦4 ♥5 ♠5 ♦5",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAIN)},
    {"♣3 ♠3 ♦3 ♥4 ♠4 ♦4 ♦6 ♦9",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, HAND_CHAIN)},
    {"♣3 ♠3 ♦3 ♥4 ♠4 ♦4 ♦6 ♠6 ♦9 ♠9",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, HAND_CHAIN)},
    {"♣4 ♠4 ♦4 ♥4 ♠5 ♦6",
     Hand_Format(HAND_PRIMAL_FOUR, HAND_KICKER_DUAL_SOLO, HAND_CHAINLESS)},
    {"♣3 ♠3 ♦3 ♥3 ♠6 ♦6 ♦9 ♠9",
     Hand_Format(HAND_PRIMAL_FOUR, HAND_KICKER_DUAL_PAIR, HAND_CHAINLESS)},
    /* not a hand */
    {"♣3 ♠4", HAND_NONE},
    {"♣3 ♠4 ♦5 ♥6", HAND_NONE},
    {"♣3 ♠3 ♦4 ♥4", HAND_NONE},
    {"♣3 ♠4 ♦5 ♥6 ♠7 ♦8 ♦9 ♦T ♦J ♦Q ♦K ♦A ♦2 ♦r ♦R", HAND_NONE},
};

static void test_rules(void) {
  size_t i;

  printf("testing rules...\n");
  for (i = 0; i < sizeof(parse_cases) / sizeof(parse_cases[0]); i++) {
    card_array_t cards;
    hand_t hand;
    int type;

    CardArray_InitFromString(&cards, parse_cases[i].cards);
    type = Hand_Parse(&hand, &cards);
    if (type != parse_cases[i].type) {
      printf(
          "  [%s] parsed as 0x%02x, expected 0x%02x\n", parse_cases[i].cards,
          (unsigned)type, (unsigned)parse_cases[i].type);
      assert(0);
    }
  }
}

/* ************************************************************
 * whole game: seed -> one finished game
 * ************************************************************/

#define TEST_SEED_BEGIN 10000
#define TEST_SEED_END 10200

/* same cards in the same order */
static int same_sequence(const card_array_t *a, const card_array_t *b) {
  return a->length == b->length &&
         memcmp(a->cards, b->cards, (size_t)a->length) == 0;
}

/* the same seed must give the same game, whatever was played before it */
static void test_determinism(void) {
  game_t game;
  game_t first;
  uint32_t seed;

  printf("testing determinism...\n");
  Game_Init(&game);
  for (seed = TEST_SEED_BEGIN; seed < TEST_SEED_BEGIN + 20; seed++) {
    Game_Play(&game, seed);
    first = game;
    Game_Reset(&game);

    /* an unrelated game in between must not matter */
    Game_Play(&game, seed + 12345);
    Game_Reset(&game);

    Game_Play(&game, seed);
    if (game.winner != first.winner || game.landlord != first.landlord ||
        game.bid != first.bid ||
        !same_sequence(&game.cardRecord, &first.cardRecord)) {
      printf("  seed %u played differently the second time\n", (unsigned)seed);
      assert(0);
    }
    Game_Reset(&game);
  }
  Game_Clear(&game);
}

static void test_games(void) {
  game_t game;
  uint32_t seed;

  printf("testing games...\n");
  Game_Init(&game);
  for (seed = TEST_SEED_BEGIN; seed < TEST_SEED_END; seed++) {
    Game_Play(&game, seed);
    if (game.status != GameStatus_Over || game.winner < 0 ||
        game.winner >= GAME_PLAYERS ||
        game.players[game.winner].cards.length != 0) {
      printf("  seed %u did not finish with a winner\n", (unsigned)seed);
      assert(0);
    }
    Game_Reset(&game);
  }
  Game_Clear(&game);
}

int main(void) {
  test_rules();
  test_games();
  test_determinism();
  printf("landlord: all tests passed\n");
  return 0;
}
