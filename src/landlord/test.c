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
    /* chains may end at the ace */
    {"♠T ♠J ♠Q ♠K ♠A",
     Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAIN)},
    {"♠3 ♠4 ♠5 ♠6 ♠7 ♠8 ♠9 ♠T ♠J ♠Q ♠K ♠A",
     Hand_Format(HAND_PRIMAL_SOLO, HAND_KICKER_NONE, HAND_CHAIN)},
    {"♠Q ♥Q ♠K ♥K ♠A ♥A",
     Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAIN)},
    {"♠K ♥K ♦K ♠A ♥A ♦A",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_NONE, HAND_CHAIN)},
    {"♠K ♥K ♦K ♠A ♥A ♦A ♠3 ♠2",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, HAND_CHAIN)},
    /* any number of trios, each with a solo or each with a pair */
    {"♣J ♦J ♥J ♣T ♦T ♥T ♣9 ♦9 ♥9 ♠K ♦K ♦8 ♣8 ♥6 ♣6",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, HAND_CHAIN)},
    {"♣3 ♦3 ♥3 ♣4 ♦4 ♥4 ♣5 ♦5 ♥5 ♣6 ♦6 ♥6 ♠8 ♦8 ♠9 ♦9 ♠J ♦J ♠K ♦K",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, HAND_CHAIN)},
    {"♣3 ♦3 ♥3 ♣4 ♦4 ♥4 ♣5 ♦5 ♥5 ♠8 ♠9 ♠r",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, HAND_CHAIN)},
    {"♣3 ♦3 ♥3 ♣4 ♦4 ♥4 ♣5 ♦5 ♥5 ♣6 ♦6 ♥6 ♣7 ♦7 ♥7 ♠9 ♠T ♠J ♠K ♠2",
     Hand_Format(HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, HAND_CHAIN)},
    /* the same card twice */
    {"♣K ♣K", HAND_NONE},
    {"♠9 ♥9 ♣9 ♠8 ♥8 ♣8 ♣K ♣K", HAND_NONE},
    {"♣3 ♣4 ♠5 ♠6 ♥7 ♥7", HAND_NONE},
    /* 2 and jokers never chain */
    {"♠J ♠Q ♠K ♠A ♠2", HAND_NONE},
    {"♠K ♥K ♠A ♥A ♠2 ♥2", HAND_NONE},
    {"♠A ♥A ♦A ♠2 ♥2 ♦2", HAND_NONE},
    {"♠Q ♠K ♠A ♠2 ♠r ♠R", HAND_NONE},
    /* chains that are too short or broken */
    {"♣3 ♣4 ♠5 ♠6", HAND_NONE},
    {"♣3 ♣4 ♠5 ♠6 ♥8", HAND_NONE},
    {"♣3 ♠3 ♦5 ♥5 ♠6 ♦6", HAND_NONE},
    {"♣3 ♠3 ♦3 ♥5 ♠5 ♦5", HAND_NONE},
    {"♣3 ♠3 ♦3 ♥5 ♠5 ♦5 ♠8 ♠9", HAND_NONE},
    /* kickers that do not match the trios */
    {"♣3 ♠3 ♦3 ♥4 ♠4 ♦4 ♦6", HAND_NONE},
    {"♣3 ♠3 ♦3 ♥4 ♠4 ♦4 ♦6 ♠6 ♦9", HAND_NONE},
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

static hand_t parse(const char *str) {
  card_array_t cards;
  hand_t hand;

  CardArray_InitFromString(&cards, str);
  Hand_Parse(&hand, &cards);
  return hand;
}

/* parsing must not reorder or change the caller's cards */
static void test_parse_keeps_input(void) {
  card_array_t cards;
  card_array_t before;
  hand_t hand;

  CardArray_InitFromString(&cards, "♥7 ♣3 ♠3 ♥3");
  CardArray_Copy(&before, &cards);
  assert(Hand_Parse(&hand, &cards) != HAND_NONE);
  assert(memcmp(&cards, &before, sizeof(card_array_t)) == 0);

  /* the trio leads the parsed hand */
  assert(hand.cards.length == 4);
  assert(CARD_RANK(hand.cards.cards[0]) == CARD_RANK_3);
  assert(CARD_RANK(hand.cards.cards[3]) == CARD_RANK_7);
}

typedef struct {
  const char *a;
  const char *b;
  int result; /* Hand_Compare(a, b) */
} compare_case_t;

static const compare_case_t compare_cases[] = {
    /* same type, higher rank wins */
    {"♣4", "♣3", HAND_CMP_GREATER},
    {"♣3", "♣4", HAND_CMP_LESS},
    {"♣3", "♠3", HAND_CMP_EQUAL},
    {"♣2", "♣A", HAND_CMP_GREATER},
    {"♣R", "♣r", HAND_CMP_GREATER},
    {"♣r", "♣2", HAND_CMP_GREATER},
    {"♣5 ♠5", "♣4 ♠4", HAND_CMP_GREATER},
    {"♣4 ♠4 ♦4 ♥9", "♣5 ♠5 ♦5 ♥3", HAND_CMP_LESS},
    {"♣4 ♣5 ♠6 ♠7 ♥8", "♣3 ♣4 ♠5 ♠6 ♥7", HAND_CMP_GREATER},
    {"♠T ♠J ♠Q ♠K ♠A", "♠9 ♠T ♠J ♠Q ♠K", HAND_CMP_GREATER},
    /* bombs beat everything but bigger bombs, the nuke beats all */
    {"♣3 ♠3 ♦3 ♥3", "♣2 ♠2", HAND_CMP_GREATER},
    {"♣2 ♠2", "♣3 ♠3 ♦3 ♥3", HAND_CMP_LESS},
    {"♣3 ♠3 ♦3 ♥3", "♠T ♠J ♠Q ♠K ♠A", HAND_CMP_GREATER},
    {"♣4 ♠4 ♦4 ♥4", "♣3 ♠3 ♦3 ♥3", HAND_CMP_GREATER},
    {"♣3 ♠3 ♦3 ♥3", "♣4 ♠4 ♦4 ♥4", HAND_CMP_LESS},
    {"♠r ♠R", "♣2 ♠2 ♦2 ♥2", HAND_CMP_GREATER},
    {"♣2 ♠2 ♦2 ♥2", "♠r ♠R", HAND_CMP_LESS},
    {"♠r ♠R", "♣3", HAND_CMP_GREATER},
    /* different types or lengths can not be compared */
    {"♣5 ♠5", "♣4", HAND_CMP_ILLEGAL},
    {"♣5 ♠5 ♦5", "♣4 ♠4", HAND_CMP_ILLEGAL},
    {"♣4 ♣5 ♠6 ♠7 ♥8 ♥9", "♣3 ♣4 ♠5 ♠6 ♥7", HAND_CMP_ILLEGAL},
    {"♣5 ♠5 ♦5 ♥9", "♣4 ♠4 ♦4", HAND_CMP_ILLEGAL},
};

static void test_compare(void) {
  size_t i;

  printf("testing compare...\n");
  for (i = 0; i < sizeof(compare_cases) / sizeof(compare_cases[0]); i++) {
    hand_t a = parse(compare_cases[i].a);
    hand_t b = parse(compare_cases[i].b);
    int result;

    assert(a.type != HAND_NONE && b.type != HAND_NONE);
    result = Hand_Compare(&a, &b);
    if (result != compare_cases[i].result) {
      printf(
          "  [%s] against [%s] is %d, expected %d\n", compare_cases[i].a,
          compare_cases[i].b, result, compare_cases[i].result);
      assert(0);
    }
  }
}

/* ************************************************************
 * card array: operations on a full 54 card array stay inside it
 * ************************************************************/

static void test_card_array(void) {
  /* on the heap so that a sanitizer sees any access past the end */
  card_array_t *full = malloc(sizeof(card_array_t));
  card_array_t reference;
  int i;

  printf("testing card array...\n");
  CardArray_Reset(&reference);

  CardArray_Reset(full);
  for (i = 0; i < CARD_SET_LENGTH; i++) {
    assert(CardArray_PopFront(full) == reference.cards[i]);
    assert(full->length == CARD_SET_LENGTH - 1 - i);
  }
  assert(CardArray_PopFront(full) == 0);

  CardArray_Reset(full);
  assert(CardArray_DropFront(full, 4) == 4);
  assert(full->length == CARD_SET_LENGTH - 4);
  assert(full->cards[0] == reference.cards[4]);
  assert(full->cards[full->length - 1] == reference.cards[CARD_SET_LENGTH - 1]);
  assert(CardArray_DropFront(full, CARD_SET_LENGTH) == CARD_SET_LENGTH - 4);
  assert(full->length == 0);

  CardArray_Reset(full);
  CardArray_PushBack(full, reference.cards[0]); /* full, must be ignored */
  assert(full->length == CARD_SET_LENGTH);
  assert(CardArray_PushFront(full, reference.cards[0]) == 0);
  assert(CardArray_PopBack(full) == reference.cards[CARD_SET_LENGTH - 1]);

  free(full);
}

/* ************************************************************
 * beat search: every hand it offers beats the hand it answers
 * ************************************************************/

static void test_beat_search(void) {
  mt19937_t mt;
  deck_t deck;
  int round;
  int offered = 0;

  printf("testing beat search...\n");
  Random_Init(&mt, 2014);

  for (round = 0; round < 3000; round++) {
    card_array_t mine;
    card_array_t theirs;
    rk_list_t *lead;
    rk_list_node_t *leadnode;

    Deck_Reset(&deck);
    Deck_Shuffle(&deck, &mt);
    Deck_Deal(&deck, &mine, 20);
    Deck_Deal(&deck, &theirs, 17);

    /* answer every hand the other player could lead */
    lead = HandList_StandardAnalyze(&theirs);
    for (leadnode = lead->first; leadnode != NULL; leadnode = leadnode->next) {
      hand_t tobeat;
      rk_list_t *beats;
      rk_list_node_t *node;

      assert(
          Hand_Parse(&tobeat, &HandList_GetHand(leadnode)->cards) != HAND_NONE);
      beats = HandList_SearchBeatList(&mine, &tobeat);

      for (node = beats->first; node != NULL; node = node->next) {
        hand_t beat;

        assert(Hand_Parse(&beat, &HandList_GetHand(node)->cards) != HAND_NONE);
        assert(Hand_Compare(&beat, &tobeat) == HAND_CMP_GREATER);
        assert(CardArray_IsContain(&mine, &beat.cards));
        offered++;
      }

      rk_list_clear_destroy(beats);
    }

    rk_list_clear_destroy(lead);
  }

  assert(offered > 0);
}

/* ************************************************************
 * whole game: seed -> one finished game
 * ************************************************************/

#define TEST_SEED_BEGIN 10000
#define TEST_SEED_END 11000

/* ************************************************************
 * baseline: what every seed played like when it was last recorded
 * ************************************************************/

typedef struct {
  int winner;
  int landlord;
  int bid;
  uint32_t plays; /* FNV-1a of every card played, in order */
} game_summary_t;

/* generated by `make landlord-baseline`, do not edit */
static const game_summary_t baseline[] = {
#include "baseline.c.inc"
};

static game_summary_t summarize(const game_t *game) {
  game_summary_t summary;
  int i;

  summary.winner = game->winner;
  summary.landlord = game->landlord;
  summary.bid = game->bid;
  summary.plays = 2166136261u;
  for (i = 0; i < game->cardRecord.length; i++)
    summary.plays = (summary.plays ^ game->cardRecord.cards[i]) * 16777619u;

  return summary;
}

static void print_baseline(void) {
  game_t game;
  uint32_t seed;

  Game_Init(&game);
  for (seed = TEST_SEED_BEGIN; seed < TEST_SEED_END; seed++) {
    game_summary_t s;

    Game_Play(&game, seed);
    s = summarize(&game);
    printf(
        "{%d, %d, %d, 0x%08xu}, /* %u */\n", s.winner, s.landlord, s.bid,
        (unsigned)s.plays, (unsigned)seed);
    Game_Reset(&game);
  }
  Game_Clear(&game);
}

static void test_baseline(void) {
  game_t game;
  uint32_t seed;

  printf("testing baseline...\n");
  assert(
      sizeof(baseline) / sizeof(baseline[0]) ==
      TEST_SEED_END - TEST_SEED_BEGIN);

  Game_Init(&game);
  for (seed = TEST_SEED_BEGIN; seed < TEST_SEED_END; seed++) {
    const game_summary_t *want = &baseline[seed - TEST_SEED_BEGIN];
    game_summary_t got;

    Game_Play(&game, seed);
    got = summarize(&game);
    if (got.landlord != want->landlord || got.bid != want->bid) {
      printf(
          "  seed %u: bidding differs, landlord %d bid %d, baseline %d bid "
          "%d\n",
          (unsigned)seed, got.landlord, got.bid, want->landlord, want->bid);
      assert(0);
    }
    if (got.plays != want->plays) {
      printf(
          "  seed %u: cards played differ, 0x%08x, baseline 0x%08x\n",
          (unsigned)seed, (unsigned)got.plays, (unsigned)want->plays);
      assert(0);
    }
    if (got.winner != want->winner) {
      printf(
          "  seed %u: winner %d, baseline %d\n", (unsigned)seed, got.winner,
          want->winner);
      assert(0);
    }
    Game_Reset(&game);
  }
  Game_Clear(&game);
}

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

/* played cards plus the cards still held are exactly one deck */
static int is_whole_deck(const game_t *game) {
  card_array_t all;
  card_array_t deck;
  int i;

  CardArray_Copy(&all, &game->cardRecord);
  for (i = 0; i < GAME_PLAYERS; i++) {
    card_array_t held;

    /* a full array silently ignores what does not fit, count first */
    if (all.length + game->players[i].cards.length > CARD_SET_LENGTH)
      return 0;

    CardArray_Copy(&held, &game->players[i].cards);
    CardArray_Concat(&all, &held);
  }

  CardArray_Reset(&deck);
  CardArray_Sort(&deck, NULL);
  CardArray_Sort(&all, NULL);

  return same_sequence(&all, &deck);
}

/* an analysis that calls two unrelated cards one hand, the AI will lead it */
static rk_list_t *cheat_analyze(card_array_t *cards) {
  rk_list_t *hl = rk_list_create();
  hand_t hand;

  Hand_Clear(&hand);
  hand.type = Hand_Format(HAND_PRIMAL_PAIR, HAND_KICKER_NONE, HAND_CHAINLESS);
  CardArray_PushBack(&hand.cards, cards->cards[0]);
  CardArray_PushBack(&hand.cards, cards->cards[cards->length - 1]);
  HandList_PushFront(hl, &hand);

  return hl;
}

static void test_game_rejects_illegal_hands(void) {
  const ai_t cheat = {cheat_analyze, HandList_StandardEvaluator};
  game_t game;
  int i;

  printf("testing that an illegal hand stops the game...\n");
  printf("  (one rejection message is expected below)\n");
  fflush(stdout);

  Game_Init(&game);
  for (i = 0; i < GAME_PLAYERS; i++)
    game.players[i].ai = &cheat;

  Game_Play(&game, TEST_SEED_BEGIN);
  assert(game.status == GameStatus_Illegal);
  assert(game.cardRecord.length == 0);

  Game_Clear(&game);
}

/* ************************************************************
 * AI: a position in, a decision out, no game needed
 * ************************************************************/

static void test_ai_decides_from_a_view(void) {
  card_array_t cards;
  card_array_t played;
  hand_t last = parse("♣3 ♦3");
  hand_t decision;
  ai_view_t view;

  printf("testing AI decisions...\n");
  CardArray_InitFromString(&cards, "♠9 ♠4 ♥4");
  CardArray_Clear(&played);

  memset(&view, 0, sizeof(view));
  view.ai = &AI_Standard;
  view.seat = 1;
  view.landlord = 0;
  view.cards = &cards;
  view.lastHand = &last;
  view.played = &played;
  view.cardsLeft[0] = 10;
  view.cardsLeft[1] = cards.length;
  view.cardsLeft[2] = 2;

  /* the landlord led a pair of 3, the peasant answers with its pair of 4 */
  view.lastPlayer = 0;
  assert(AI_Beat(&view, &decision) == 1);
  assert(decision.cards.length == 2);
  assert(CARD_RANK(decision.cards.cards[0]) == CARD_RANK_4);
  assert(CARD_RANK(decision.cards.cards[1]) == CARD_RANK_4);

  /* the same pair from a teammate who is closer to going out: pass */
  view.lastPlayer = 2;
  assert(AI_Beat(&view, &decision) == 0);

  /* deciding changes nothing it was shown */
  assert(cards.length == 3);

  /* nothing in hand beats a pair of 2 */
  last = parse("♣2 ♦2");
  view.lastPlayer = 0;
  assert(AI_Beat(&view, &decision) == 0);
}

static void test_games(void) {
  game_t game;
  uint32_t seed;

  printf("testing games...\n");
  Game_Init(&game);
  for (seed = TEST_SEED_BEGIN; seed < TEST_SEED_END; seed++) {
    Game_Play(&game, seed);

    /*
     * the game itself refuses any hand that is not legal, does not beat the
     * last one or is not made of the player's own cards, and stops there
     */
    if (game.status != GameStatus_Over || game.winner < 0 ||
        game.winner >= GAME_PLAYERS ||
        game.players[game.winner].cards.length != 0) {
      printf("  seed %u did not finish with a winner\n", (unsigned)seed);
      assert(0);
    }
    if (!is_whole_deck(&game)) {
      printf("  seed %u lost or duplicated cards\n", (unsigned)seed);
      assert(0);
    }
    Game_Reset(&game);
  }
  Game_Clear(&game);
}

int main(int argc, char **argv) {
  /* regenerate the baseline instead of testing against it */
  if (argc > 1 && strcmp(argv[1], "--baseline") == 0) {
    print_baseline();
    return 0;
  }

  test_rules();
  test_parse_keeps_input();
  test_compare();
  test_card_array();
  test_beat_search();
  test_ai_decides_from_a_view();
  test_games();
  test_game_rejects_illegal_hands();
  test_determinism();
  test_baseline();
  printf("landlord: all tests passed\n");
  return 0;
}
