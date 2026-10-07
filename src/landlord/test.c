/*
 * landlord self-check: run by `make test`, exits non-zero on failure.
 *
 * Two seams are tested: the rules (cards -> hand type) and a whole game
 * (seed -> one finished game).
 */

#undef NDEBUG
#include <assert.h>

#include "landlord.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * The cases below spell cards with suit symbols. A build with
 * LANDLORD_ASCII_SUITS reads c d h s instead, so translate on the way in.
 */
#ifdef LANDLORD_ASCII_SUITS
#define CLUB "c"
#define DIAMOND "d"
#define HEART "h"
#define SPADE "s"
#else
#define CLUB "♣"
#define DIAMOND "♦"
#define HEART "♥"
#define SPADE "♠"
#endif

static void cards_from_text(card_array_t *cards, const char *text) {
  static const char *const symbols[] = {"♣", "♦", "♥", "♠"};
  static const char *const suits[] = {CLUB, DIAMOND, HEART, SPADE};
  char translated[256];
  size_t used = 0;

  while (*text != '\0') {
    size_t i;
    size_t n = sizeof(symbols) / sizeof(symbols[0]);

    for (i = 0; i < n; i++) {
      if (strncmp(text, symbols[i], strlen(symbols[i])) == 0)
        break;
    }

    assert(used + 4 < sizeof(translated));
    if (i < n) {
      memcpy(translated + used, suits[i], strlen(suits[i]));
      used += strlen(suits[i]);
      text += strlen(symbols[i]);
    } else {
      translated[used++] = *text++;
    }
  }

  translated[used] = '\0';
  card_array_init_from_string(cards, translated);
}

/* ************************************************************
 * rules: cards -> hand type
 * ************************************************************/

typedef struct {
  const char *cards;
  hand_type_t type;
} parse_case_t;

#define NOT_A_HAND {HAND_PRIMAL_NONE, HAND_KICKER_NONE, false}

static const parse_case_t parse_cases[] = {
    {"♣3", {HAND_PRIMAL_SOLO, HAND_KICKER_NONE, false}},
    {"♣3 ♠3", {HAND_PRIMAL_PAIR, HAND_KICKER_NONE, false}},
    {"♠r ♠R", {HAND_PRIMAL_NUKE, HAND_KICKER_NONE, false}},
    {"♠6 ♥6 ♦6", {HAND_PRIMAL_TRIO, HAND_KICKER_NONE, false}},
    {"♣3 ♠3 ♥3 ♥7", {HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, false}},
    {"♣3 ♣5 ♠5 ♠3 ♥5", {HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, false}},
    {"♣7 ♠7 ♥7 ♦7", {HAND_PRIMAL_BOMB, HAND_KICKER_NONE, false}},
    {"♣3 ♣4 ♠5 ♠6 ♥7", {HAND_PRIMAL_SOLO, HAND_KICKER_NONE, true}},
    {"♣3 ♠4 ♦6 ♥8 ♠7 ♦5 ♦9 ♦T ♦J", {HAND_PRIMAL_SOLO, HAND_KICKER_NONE, true}},
    {"♣4 ♠4 ♠5 ♥6 ♥5 ♦6", {HAND_PRIMAL_PAIR, HAND_KICKER_NONE, true}},
    {"♣4 ♠4 ♦4 ♥5 ♠5 ♦5", {HAND_PRIMAL_TRIO, HAND_KICKER_NONE, true}},
    {"♣3 ♠3 ♦3 ♥4 ♠4 ♦4 ♦6 ♦9", {HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, true}},
    {"♣3 ♠3 ♦3 ♥4 ♠4 ♦4 ♦6 ♠6 ♦9 ♠9",
     {HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, true}},
    {"♣4 ♠4 ♦4 ♥4 ♠5 ♦6", {HAND_PRIMAL_FOUR, HAND_KICKER_DUAL_SOLO, false}},
    {"♣3 ♠3 ♦3 ♥3 ♠6 ♦6 ♦9 ♠9",
     {HAND_PRIMAL_FOUR, HAND_KICKER_DUAL_PAIR, false}},
    /* chains may end at the ace */
    {"♠T ♠J ♠Q ♠K ♠A", {HAND_PRIMAL_SOLO, HAND_KICKER_NONE, true}},
    {"♠3 ♠4 ♠5 ♠6 ♠7 ♠8 ♠9 ♠T ♠J ♠Q ♠K ♠A",
     {HAND_PRIMAL_SOLO, HAND_KICKER_NONE, true}},
    {"♠Q ♥Q ♠K ♥K ♠A ♥A", {HAND_PRIMAL_PAIR, HAND_KICKER_NONE, true}},
    {"♠K ♥K ♦K ♠A ♥A ♦A", {HAND_PRIMAL_TRIO, HAND_KICKER_NONE, true}},
    {"♠K ♥K ♦K ♠A ♥A ♦A ♠3 ♠2", {HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, true}},
    /* any number of trios, each with a solo or each with a pair */
    {"♣J ♦J ♥J ♣T ♦T ♥T ♣9 ♦9 ♥9 ♠K ♦K ♦8 ♣8 ♥6 ♣6",
     {HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, true}},
    {"♣3 ♦3 ♥3 ♣4 ♦4 ♥4 ♣5 ♦5 ♥5 ♣6 ♦6 ♥6 ♠8 ♦8 ♠9 ♦9 ♠J ♦J ♠K ♦K",
     {HAND_PRIMAL_TRIO, HAND_KICKER_PAIR, true}},
    {"♣3 ♦3 ♥3 ♣4 ♦4 ♥4 ♣5 ♦5 ♥5 ♠8 ♠9 ♠r",
     {HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, true}},
    {"♣3 ♦3 ♥3 ♣4 ♦4 ♥4 ♣5 ♦5 ♥5 ♣6 ♦6 ♥6 ♣7 ♦7 ♥7 ♠9 ♠T ♠J ♠K ♠2",
     {HAND_PRIMAL_TRIO, HAND_KICKER_SOLO, true}},
    /* the same card twice */
    {"♣K ♣K", NOT_A_HAND},
    {"♠9 ♥9 ♣9 ♠8 ♥8 ♣8 ♣K ♣K", NOT_A_HAND},
    {"♣3 ♣4 ♠5 ♠6 ♥7 ♥7", NOT_A_HAND},
    /* 2 and jokers never chain */
    {"♠J ♠Q ♠K ♠A ♠2", NOT_A_HAND},
    {"♠K ♥K ♠A ♥A ♠2 ♥2", NOT_A_HAND},
    {"♠A ♥A ♦A ♠2 ♥2 ♦2", NOT_A_HAND},
    {"♠Q ♠K ♠A ♠2 ♠r ♠R", NOT_A_HAND},
    /* chains that are too short or broken */
    {"♣3 ♣4 ♠5 ♠6", NOT_A_HAND},
    {"♣3 ♣4 ♠5 ♠6 ♥8", NOT_A_HAND},
    {"♣3 ♠3 ♦5 ♥5 ♠6 ♦6", NOT_A_HAND},
    {"♣3 ♠3 ♦3 ♥5 ♠5 ♦5", NOT_A_HAND},
    {"♣3 ♠3 ♦3 ♥5 ♠5 ♦5 ♠8 ♠9", NOT_A_HAND},
    /* kickers that do not match the trios */
    {"♣3 ♠3 ♦3 ♥4 ♠4 ♦4 ♦6", NOT_A_HAND},
    {"♣3 ♠3 ♦3 ♥4 ♠4 ♦4 ♦6 ♠6 ♦9", NOT_A_HAND},
    /* not a hand */
    {"♣3 ♠4", NOT_A_HAND},
    {"♣3 ♠4 ♦5 ♥6", NOT_A_HAND},
    {"♣3 ♠3 ♦4 ♥4", NOT_A_HAND},
    {"♣3 ♠4 ♦5 ♥6 ♠7 ♦8 ♦9 ♦T ♦J ♦Q ♦K ♦A ♦2 ♦r ♦R", NOT_A_HAND},
};

static void test_rules(void) {
  size_t i;

  printf("testing rules...\n");
  for (i = 0; i < sizeof(parse_cases) / sizeof(parse_cases[0]); i++) {
    const hand_type_t want = parse_cases[i].type;
    card_array_t cards;
    hand_t hand;
    bool ishand;

    cards_from_text(&cards, parse_cases[i].cards);
    ishand = hand_parse(&hand, &cards);

    /* not a hand: no type; a hand: exactly the expected type */
    if ((ishand != (want.primal != HAND_PRIMAL_NONE)) ||
        !hand_type_equals(hand.type, want)) {
      printf(
          "  [%s] parsed as primal %d kicker %d chain %d, expected primal %d "
          "kicker %d chain %d\n",
          parse_cases[i].cards, (int)hand.type.primal, (int)hand.type.kicker,
          (int)hand.type.chain, (int)want.primal, (int)want.kicker,
          (int)want.chain);
      assert(0);
    }
  }
}

/* same cards in the same order */
static int same_sequence(const card_array_t *a, const card_array_t *b) {
  int i;

  if (card_array_length(a) != card_array_length(b))
    return 0;

  for (i = 0; i < card_array_length(a); i++) {
    if (card_array_at(a, i) != card_array_at(b, i))
      return 0;
  }

  return 1;
}

static hand_t parse(const char *str) {
  card_array_t cards;
  hand_t hand;

  cards_from_text(&cards, str);
  hand_parse(&hand, &cards);
  return hand;
}

/* parsing must not reorder or change the caller's cards */
static void test_parse_keeps_input(void) {
  card_array_t cards;
  card_array_t before;
  hand_t hand;

  cards_from_text(&cards, "♥7 ♣3 ♠3 ♥3");
  card_array_copy(&before, &cards);
  assert(hand_parse(&hand, &cards));
  assert(memcmp(&cards, &before, sizeof(card_array_t)) == 0);

  /* the trio leads the parsed hand */
  assert(card_array_length(&hand.cards) == 4);
  assert(CARD_RANK(card_array_at(&hand.cards, 0)) == CARD_RANK_3);
  assert(CARD_RANK(card_array_at(&hand.cards, 3)) == CARD_RANK_7);
}

typedef struct {
  const char *a;
  const char *b;
  hand_compare_t result; /* hand_compare(a, b) */
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
    hand_compare_t result;

    assert(!hand_is_none(&a) && !hand_is_none(&b));
    result = hand_compare(&a, &b);
    if (result != compare_cases[i].result) {
      printf(
          "  [%s] against [%s] is %d, expected %d\n", compare_cases[i].a,
          compare_cases[i].b, (int)result, (int)compare_cases[i].result);
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
  card_array_reset(&reference);

  card_array_reset(full);
  for (i = 0; i < CARD_SET_LENGTH; i++) {
    assert(card_array_pop_front(full) == card_array_at(&reference, i));
    assert(card_array_length(full) == CARD_SET_LENGTH - 1 - i);
  }
  assert(card_array_pop_front(full) == 0);

  card_array_reset(full);
  assert(card_array_drop_front(full, 4) == 4);
  assert(card_array_length(full) == CARD_SET_LENGTH - 4);
  assert(card_array_at(full, 0) == card_array_at(&reference, 4));
  assert(
      card_array_at(full, card_array_length(full) - 1) ==
      card_array_at(&reference, CARD_SET_LENGTH - 1));
  assert(card_array_drop_front(full, CARD_SET_LENGTH) == CARD_SET_LENGTH - 4);
  assert(card_array_length(full) == 0);

  card_array_reset(full);
  card_array_push_back(
      full, card_array_at(&reference, 0)); /* full, must be ignored */
  assert(card_array_length(full) == CARD_SET_LENGTH);

  free(full);
}

static void test_card_text(void) {
  card_array_t cards;
  char str[CARD_STRING_SIZE];
  char small[2] = "x";

  printf("testing card text...\n");

  /* always terminated, and it reads back as the same card */
  card_array_init_from_string(
      &cards,
      SPADE "A " HEART "T " CLUB "3 " DIAMOND "r " SPADE "R " DIAMOND "2");
  assert(card_array_length(&cards) == 6);
  assert(
      card_to_string(card_array_at(&cards, 0), str, sizeof(str)) ==
      (int)strlen(SPADE) + 1);
  assert(strcmp(str, SPADE "A") == 0);
  assert(
      card_to_string(card_array_at(&cards, 3), str, sizeof(str)) ==
      (int)strlen(SPADE) + 1);
  assert(strcmp(str, DIAMOND "r") == 0);

  /* no room: nothing is written */
  assert(card_to_string(card_array_at(&cards, 0), small, sizeof(small)) == 0);
  assert(strcmp(small, "x") == 0);

  /* not a card: shows as '?', still terminated */
  assert(card_to_string(0, str, sizeof(str)) == 2);
  assert(strcmp(str, "??") == 0);
  assert(card_to_string(0xF3, str, sizeof(str)) == 2);
  assert(strcmp(str, "?5") == 0);

  /* anything that is not a suit or a rank is skipped */
  card_array_init_from_string(&cards, "");
  assert(card_array_length(&cards) == 0);
  card_array_init_from_string(&cards, "xyz, 1 0 ?");
  assert(card_array_length(&cards) == 0);
  card_array_init_from_string(
      &cards, CLUB "3, junk " SPADE " and 7" DIAMOND " " HEART);
  assert(card_array_length(&cards) == 2);
  assert(CARD_RANK(card_array_at(&cards, 0)) == CARD_RANK_3);
  assert(CARD_RANK(card_array_at(&cards, 1)) == CARD_RANK_7);

  /* reading past the end gives no card */
  assert(card_array_at(&cards, 2) == 0);
  assert(card_array_at(&cards, -1) == 0);
}

/* cards are asked for by rank, sorted or not */
static void test_cards_by_rank(void) {
  card_array_t cards;
  card_array_t taken;
  int count[CARD_RANK_END];

  printf("testing cards by rank...\n");
  cards_from_text(&cards, "♠5 ♣K ♥5 ♦9 ♣5 ♠K");

  card_array_count_ranks(&cards, count);
  assert(count[CARD_RANK_5] == 3 && count[CARD_RANK_K] == 2);
  assert(count[CARD_RANK_9] == 1 && count[CARD_RANK_A] == 0);

  card_array_clear(&taken);
  assert(card_array_take_rank(&taken, &cards, CARD_RANK_5, 2) == 2);
  assert(card_array_take_rank(&taken, &cards, CARD_RANK_K, 5) == 2);
  assert(card_array_take_rank(&taken, &cards, CARD_RANK_A, 1) == 0);
  assert(card_array_length(&taken) == 4);
  assert(card_array_contains(&cards, &taken));
  assert(card_array_length(&cards) == 6); /* taking copies, the source stays */

  card_array_remove_rank(&cards, CARD_RANK_5);
  assert(card_array_length(&cards) == 3);
  assert(!card_array_contains(&cards, &taken));
}

/* ************************************************************
 * hand list: a value with a capacity, it refuses instead of overflowing
 * ************************************************************/

static void test_hand_list(void) {
  /* static: two of these are too much for a test's stack to be polite */
  static hand_list_t list;
  hand_t solo = parse("♣3");
  hand_t pair = parse("♠9 ♥9");
  int i;

  printf("testing hand list...\n");
  hand_list_clear(&list);
  assert(hand_list_count(&list) == 0);
  assert(hand_list_at(&list, 0) == NULL);

  for (i = 0; i < HAND_LIST_CAPACITY; i++)
    assert(hand_list_push(&list, i % 2 ? &pair : &solo) == 1);

  /* full: the hand is refused and nothing changes */
  assert(hand_list_push(&list, &pair) == 0);
  assert(hand_list_count(&list) == HAND_LIST_CAPACITY);
  assert(hand_list_at(&list, HAND_LIST_CAPACITY) == NULL);
  assert(hand_type_equals(
      hand_list_at(&list, HAND_LIST_CAPACITY - 1)->type, pair.type));
}

/* ************************************************************
 * analysis: the hands cover the cards exactly and each one is legal
 * ************************************************************/

/* returns the number of hands */
static int check_analysis(
    void (*analyze)(const card_array_t *, hand_list_t *),
    const card_array_t *cards) {
  hand_list_t hands;
  card_array_t covered;
  card_array_t sorted;
  int i;

  analyze(cards, &hands);

  card_array_clear(&covered);
  for (i = 0; i < hand_list_count(&hands); i++) {
    const hand_t *hand = hand_list_at(&hands, i);
    hand_t judged;

    /* legal, and of the type the analysis says it is */
    assert(hand_parse(&judged, &hand->cards));
    assert(hand_type_equals(judged.type, hand->type));
    card_array_concat(&covered, &hand->cards);
  }

  /* every card in exactly one hand */
  card_array_copy(&sorted, cards);
  card_array_sort(&sorted);
  card_array_sort(&covered);
  assert(same_sequence(&covered, &sorted));

  return hand_list_count(&hands);
}

static void test_analysis(void) {
  mt19937_t mt;
  card_array_t deck;
  int round;

  printf("testing analysis...\n");
  mt19937_init(&mt, 1987);

  for (round = 0; round < 20000; round++) {
    card_array_t cards;
    int standard;
    int counted;

    card_array_reset(&deck);
    card_array_shuffle(&deck, &mt);
    card_array_deal(&deck, &cards, 1 + (int)(mt19937_int32(&mt) % 20));

    standard = check_analysis(analysis_standard, &cards);
    counted = check_analysis(analysis_counted, &cards);

    /* the search counts turns: never more than greedy takes, and with
       kickers riding along no more than the hands of its own split, which
       may well be more hands than greedy makes */
    assert(analysis_counted_hands(&cards) <= standard);
    assert(analysis_counted_hands(&cards) <= counted);
  }

  /* turns, by hand */
  {
    static const struct {
      const char *cards;
      int turns;
    } cases[] = {
        {"♠3 ♥3 ♦3 ♠4", 1},                   /* trio with a solo */
        {"♠3 ♥3 ♦3 ♠4 ♥4", 1},                /* trio with a pair */
        {"♠3 ♥3 ♦3 ♠4 ♥4 ♦4 ♠7 ♠9", 1},       /* plane with two solos */
        {"♠3 ♠4 ♠5 ♠6 ♠7 ♥7 ♠8 ♠9 ♠T ♠J", 2}, /* two chains sharing a 7 */
        {"♠3 ♥3 ♦3 ♣3 ♠5 ♠2", 3},             /* a bomb and 2 carry nothing */
    };
    size_t i;

    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
      card_array_t cards;

      cards_from_text(&cards, cases[i].cards);
      assert(analysis_counted_hands(&cards) == cases[i].turns);
    }
  }
}

/* ************************************************************
 * moves: whatever the rules allow from some cards, the generator offers a
 * move the rules call equal to it
 * ************************************************************/

static void test_moves(void) {
  mt19937_t mt;
  card_array_t deck;
  int round;

  printf("testing moves...\n");
  mt19937_init(&mt, 2026);

  /* more cards than a seat holds: the AI asks what the other two could do */
  {
    hand_list_t moves;
    hand_t solo = parse("♣3");

    card_array_reset(&deck);
    move_generate(&deck, &moves);
    assert(hand_list_count(&moves) == HAND_LIST_CAPACITY);
    move_generate_beats(&deck, &solo, &moves);
    assert(hand_list_count(&moves) > 0);
  }

  for (round = 0; round < 300; round++) {
    card_array_t cards;
    hand_list_t moves;
    unsigned subset;
    int n;
    int i;

    card_array_reset(&deck);
    card_array_shuffle(&deck, &mt);
    card_array_deal(&deck, &cards, 1 + (int)(mt19937_int32(&mt) % 12));
    n = card_array_length(&cards);

    move_generate(&cards, &moves);
    assert(hand_list_count(&moves) < HAND_LIST_CAPACITY);

    for (i = 0; i < hand_list_count(&moves); i++) {
      hand_t judged;

      assert(hand_parse(&judged, &hand_list_at(&moves, i)->cards));
      assert(hand_type_equals(judged.type, hand_list_at(&moves, i)->type));
      assert(card_array_contains(&cards, &hand_list_at(&moves, i)->cards));
    }

    /* every way to pick some of the cards */
    for (subset = 1; subset < (1u << n); subset++) {
      card_array_t picked;
      hand_t hand;
      bool offered = false;

      card_array_clear(&picked);
      for (i = 0; i < n; i++) {
        if (subset & (1u << i))
          card_array_push_back(&picked, card_array_at(&cards, i));
      }

      if (!hand_parse(&hand, &picked))
        continue;

      /* chains of fours are left out on purpose */
      if ((hand.type.primal == HAND_PRIMAL_FOUR) && hand.type.chain)
        continue;

      for (i = 0; !offered && (i < hand_list_count(&moves)); i++)
        offered =
            hand_compare(hand_list_at(&moves, i), &hand) == HAND_CMP_EQUAL;

      assert(offered);
    }
  }
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
  for (i = 0; i < card_array_length(&game->card_record); i++)
    summary.plays =
        (summary.plays ^ card_array_at(&game->card_record, i)) * 16777619u;

  return summary;
}

static void print_baseline(void) {
  game_t game;
  uint32_t seed;

  game_init(&game);
  for (seed = TEST_SEED_BEGIN; seed < TEST_SEED_END; seed++) {
    game_summary_t s;

    game_play(&game, seed);
    s = summarize(&game);
    printf(
        "{%d, %d, %d, 0x%08xu}, /* %u */\n", s.winner, s.landlord, s.bid,
        (unsigned)s.plays, (unsigned)seed);
  }
}

static void test_baseline(void) {
  game_t game;
  uint32_t seed;

  printf("testing baseline...\n");
  assert(
      sizeof(baseline) / sizeof(baseline[0]) ==
      TEST_SEED_END - TEST_SEED_BEGIN);

  game_init(&game);
  for (seed = TEST_SEED_BEGIN; seed < TEST_SEED_END; seed++) {
    const game_summary_t *want = &baseline[seed - TEST_SEED_BEGIN];
    game_summary_t got;

    game_play(&game, seed);
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
  }
}

/* the same seed must give the same game, whatever was played before it */
static void test_determinism(void) {
  game_t game;
  game_t first;
  uint32_t seed;

  printf("testing determinism...\n");
  game_init(&game);
  for (seed = TEST_SEED_BEGIN; seed < TEST_SEED_BEGIN + 20; seed++) {
    game_play(&game, seed);
    first = game;

    /* an unrelated game in between must not matter */
    game_play(&game, seed + 12345);

    game_play(&game, seed);
    if (game.winner != first.winner || game.landlord != first.landlord ||
        game.bid != first.bid ||
        !same_sequence(&game.card_record, &first.card_record)) {
      printf("  seed %u played differently the second time\n", (unsigned)seed);
      assert(0);
    }
  }
}

/* played cards plus the cards still held are exactly one deck */
static int is_whole_deck(const game_t *game) {
  card_array_t all;
  card_array_t deck;
  int i;

  card_array_copy(&all, &game->card_record);
  for (i = 0; i < GAME_PLAYERS; i++) {
    card_array_t held;

    /* a full array silently ignores what does not fit, count first */
    if (card_array_length(&all) + card_array_length(&game->players[i].cards) >
        CARD_SET_LENGTH)
      return 0;

    card_array_copy(&held, &game->players[i].cards);
    card_array_concat(&all, &held);
  }

  card_array_reset(&deck);
  card_array_sort(&deck);
  card_array_sort(&all);

  return same_sequence(&all, &deck);
}

/* an AI that leads two unrelated cards as if they were a hand */
static void cheat_lead(const ai_view_t *view, hand_t *hand) {
  card_array_push_back(&hand->cards, card_array_at(view->cards, 0));
  card_array_push_back(
      &hand->cards,
      card_array_at(view->cards, card_array_length(view->cards) - 1));
}

static bool cheat_beat(const ai_view_t *view, hand_t *hand) {
  (void)view;
  (void)hand;
  return false;
}

static void test_game_rejects_illegal_hands(void) {
  const ai_t cheat = {cheat_lead, cheat_beat};
  game_t game;
  int i;

  printf("testing that an illegal hand stops the game...\n");
  printf("  (one rejection message is expected below)\n");
  fflush(stdout);

  game_init(&game);
  for (i = 0; i < GAME_PLAYERS; i++)
    game.players[i].ai = &cheat;

  game_play(&game, TEST_SEED_BEGIN);
  assert(game.status == GAME_STATUS_ILLEGAL);
  assert(card_array_length(&game.card_record) == 0);
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
  cards_from_text(&cards, "♠9 ♠4 ♥4");
  card_array_clear(&played);

  memset(&view, 0, sizeof(view));
  view.ai = &ai_moves;
  view.seat = 1;
  view.landlord = 0;
  view.cards = &cards;
  view.last_hand = &last;
  view.played = &played;
  view.cards_left[0] = 10;
  view.cards_left[1] = card_array_length(&cards);
  view.cards_left[2] = 2;

  /* the landlord led a pair of 3, the peasant answers with its pair of 4 */
  view.last_player = 0;
  assert(ai_beat(&view, &decision) == 1);
  assert(card_array_length(&decision.cards) == 2);
  assert(CARD_RANK(card_array_at(&decision.cards, 0)) == CARD_RANK_4);
  assert(CARD_RANK(card_array_at(&decision.cards, 1)) == CARD_RANK_4);

  /* the same pair from a teammate who is closer to going out: pass */
  view.last_player = 2;
  assert(ai_beat(&view, &decision) == 0);

  /* deciding changes nothing it was shown */
  assert(card_array_length(&cards) == 3);

  /* nothing in hand beats a pair of 2 */
  last = parse("♣2 ♦2");
  view.last_player = 0;
  assert(ai_beat(&view, &decision) == 0);

  /* a bomb is kept while something cheaper beats the hand */
  cards_from_text(&cards, "♠9 ♠8 ♥8 ♦8 ♣8 ♠4 ♥4");
  last = parse("♣3 ♦3");
  assert(ai_beat(&view, &decision) == 1);
  assert(card_array_length(&decision.cards) == 2);
  assert(CARD_RANK(card_array_at(&decision.cards, 0)) == CARD_RANK_4);

  /* and spent when nothing else does */
  last = parse("♣2 ♦2");
  assert(ai_beat(&view, &decision) == 1);
  assert(hand_is_bomb(&decision));

  /* an AI choosing among every move answers the same pair with its own */
  view.ai = &ai_moves;
  cards_from_text(&cards, "♠9 ♠4 ♥4");
  last = parse("♣3 ♦3");
  assert(ai_beat(&view, &decision) == 1);
  assert(card_array_length(&decision.cards) == 2);
  assert(CARD_RANK(card_array_at(&decision.cards, 0)) == CARD_RANK_4);

  /* and leads the trio with a kicker, which leaves a single turn */
  cards_from_text(&cards, "♠3 ♥3 ♦3 ♠4 ♠5");
  view.last_hand = NULL;
  ai_lead(&view, &decision);
  assert(card_array_length(&decision.cards) == 4);
  assert(hand_rank(&decision) == CARD_RANK_3);
  view.last_hand = &last;

  /* the landlord holds one card: reading the table, lead the pair, not a
     solo it could go out on */
  view.ai = &ai_table;
  view.last_hand = NULL;
  view.cards_left[0] = 1;
  cards_from_text(&cards, "♠A ♠6 ♠4 ♥4");
  ai_lead(&view, &decision);
  assert(card_array_length(&decision.cards) == 2);

  /* nothing left can beat the pair of 2: lead it, then go out on the 3 */
  view.cards_left[0] = 3;
  cards_from_text(&played, "♣r ♦R");
  cards_from_text(&cards, "♠2 ♥2 ♠3");
  ai_lead(&view, &decision);
  assert(hand_rank(&decision) == CARD_RANK_2);

  /* an AI that does not look leads the 3 and hopes */
  view.ai = &ai_moves;
  ai_lead(&view, &decision);
  assert(hand_rank(&decision) == CARD_RANK_3);
  card_array_clear(&played);
  view.cards_left[0] = 10;
  view.last_hand = &last;

  /* of two pairs that beat, the one that leaves fewer hands: 99 leaves the
     chain 45678 whole, 55 would break it */
  cards_from_text(&cards, "♠9 ♥9 ♠8 ♠7 ♠6 ♠5 ♥5 ♠4");
  last = parse("♣3 ♦3");
  assert(ai_beat(&view, &decision) == 1);
  assert(CARD_RANK(card_array_at(&decision.cards, 0)) == CARD_RANK_9);
}

static void test_games(void) {
  game_t game;
  uint32_t seed;

  printf("testing games...\n");
  game_init(&game);
  for (seed = TEST_SEED_BEGIN; seed < TEST_SEED_END; seed++) {
    game_play(&game, seed);

    /*
     * the game itself refuses any hand that is not legal, does not beat the
     * last one or is not made of the player's own cards, and stops there
     */
    if (game.status != GAME_STATUS_OVER || game.winner < 0 ||
        game.winner >= GAME_PLAYERS ||
        card_array_length(&game.players[game.winner].cards) != 0) {
      printf("  seed %u did not finish with a winner\n", (unsigned)seed);
      assert(0);
    }
    if (!is_whole_deck(&game)) {
      printf("  seed %u lost or duplicated cards\n", (unsigned)seed);
      assert(0);
    }
  }
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
  test_card_text();
  test_cards_by_rank();
  test_hand_list();
  test_analysis();
  test_moves();
  test_ai_decides_from_a_view();
  test_games();
  test_game_rejects_illegal_hands();
  test_determinism();
  test_baseline();
  printf("landlord: all tests passed\n");
  return 0;
}
