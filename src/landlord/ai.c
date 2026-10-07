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

#include "ai.h"

#include <limits.h>

#include <stddef.h>

/*
 * How many hands the cards take apart into decides the bid. Exactly
 * AI_BID_HANDS_WEAK hands stays out as well: no branch takes it, it always
 * has been so and the baseline records it.
 */
enum {
  AI_BID_HANDS_WEAK = 9,  /* more hands than this are not worth a bid */
  AI_BID_HANDS_GOOD = 3,  /* this many is worth two */
  AI_BID_HANDS_GREAT = 2, /* this many or fewer is worth all three */
};

int ai_bid(const ai_view_t *view) {
  int shouldbid = 0;
  int handlistlen = 0;
  hand_list_t hands;

  /* the fewer hands the cards need, the more they are worth */
  analysis_standard(view->cards, &hands);
  handlistlen = hand_list_count(&hands);

  if (handlistlen > AI_BID_HANDS_WEAK) {
    shouldbid = 0;
  } else if (
      (handlistlen < AI_BID_HANDS_WEAK) && (handlistlen > AI_BID_HANDS_GOOD)) {
    shouldbid = 1;
  } else if (
      (handlistlen <= AI_BID_HANDS_GOOD) &&
      (handlistlen > AI_BID_HANDS_GREAT)) {
    shouldbid = 2;
  } else if (handlistlen <= AI_BID_HANDS_GREAT) {
    shouldbid = 3;
  }

  if (shouldbid > view->bid)
    return shouldbid;
  else
    return 0;
}

#define AI_MOVE_TURN_VALUE 100
#define AI_MOVE_BOMB_VALUE 250

/* leading what an enemy could go out on costs as much as two turns */
#define AI_TABLE_DANGER_VALUE 200
/* how many turns ahead a run to the end is looked for */
#define AI_TABLE_RUN_TURNS 3

/* what the table tells a seat, only ai_table looks at it */
typedef struct ai_table_s {
  int enemy_least;     /* cards the enemy closest to going out holds */
  int enemy_most;      /* cards the enemy furthest from it holds */
  card_array_t unseen; /* the cards the other two seats hold between them */
} ai_table_t;

static void ai_table_read(const ai_view_t *view, ai_table_t *table) {
  int seat = 0;

  table->enemy_least = INT_MAX;
  table->enemy_most = 0;

  for (seat = 0; seat < GAME_PLAYERS; seat++) {
    int left = view->cards_left[seat];

    if (seat == view->seat)
      continue;

    /* the other peasant */
    if ((view->seat != view->landlord) && (seat != view->landlord))
      continue;

    if (left < table->enemy_least)
      table->enemy_least = left;
    if (left > table->enemy_most)
      table->enemy_most = left;
  }

  card_array_reset(&table->unseen);
  card_array_subtract(&table->unseen, view->cards);
  card_array_subtract(&table->unseen, view->played);
}

/* could the cards nobody has shown yet hold a hand an enemy beats move with */
static bool ai_table_beatable(const ai_table_t *table, const hand_t *move) {
  hand_list_t beats;
  int i = 0;

  move_generate_beats(&table->unseen, move, &beats);

  for (i = 0; i < hand_list_count(&beats); i++) {
    if (card_array_length(&hand_list_at(&beats, i)->cards) <= table->enemy_most)
      return true;
  }

  return false;
}

/*
 * can cards be played out without ever losing the lead: one hand, or a move
 * nobody can beat and then the same again
 */
static bool ai_table_runs_out(
    const ai_table_t *table, const card_array_t *cards, int turns) {
  hand_list_t moves;
  int i = 0;

  if (turns <= 1)
    return true;

  move_generate(cards, &moves);

  for (i = 0; i < hand_list_count(&moves); i++) {
    const hand_t *move = hand_list_at(&moves, i);
    card_array_t rest;
    int left = 0;

    card_array_copy(&rest, cards);
    card_array_subtract(&rest, &move->cards);
    left = analysis_counted_hands(&rest);

    if ((left < turns) && !ai_table_beatable(table, move) &&
        ai_table_runs_out(table, &rest, left))
      return true;
  }

  return false;
}

/*
 * What a move costs, the lowest is played: the turns the cards left behind
 * would take, then its rank, so that high cards are kept. Spending a bomb
 * or the nuke costs extra, and going out costs nothing at all.
 *
 * With a table to look at:
 * - a solo or a pair is what an enemy holding one or two cards goes out on,
 *   so leading one costs extra and the highest is played, not the lowest
 * - a move nobody can beat wins when the rest can be run out the same way
 */
static int ai_move_value(
    const ai_view_t *view, const ai_table_t *table, const hand_t *move) {
  card_array_t rest;
  int turns = 0;
  int value = 0;
  int length = card_array_length(&move->cards);
  bool lead = view->last_hand == NULL;
  bool plain = !move->type.chain && (move->type.kicker == HAND_KICKER_NONE) &&
               (move->type.primal <= HAND_PRIMAL_PAIR);

  card_array_copy(&rest, view->cards);
  card_array_subtract(&rest, &move->cards);

  if (card_array_is_empty(&rest))
    return INT_MIN;

  turns = analysis_counted_hands(&rest);
  value = turns * AI_MOVE_TURN_VALUE + hand_rank(move);

  if (hand_is_bomb(move) || hand_is_nuke(move))
    value += AI_MOVE_BOMB_VALUE;

  if (table == NULL)
    return value;

  if (plain && (table->enemy_least == length)) {
    value -= 2 * hand_rank(move);

    if (lead)
      value += AI_TABLE_DANGER_VALUE;
  }

  if ((turns <= AI_TABLE_RUN_TURNS) && !ai_table_beatable(table, move) &&
      ai_table_runs_out(table, &rest, turns))
    return INT_MIN + 1;

  return value;
}

/* on a tie the move generated first wins */
static bool ai_moves_cheapest(
    const ai_view_t *view, const ai_table_t *table, const hand_list_t *moves,
    hand_t *hand) {
  int i = 0;
  int chosen = -1;
  int chosenvalue = 0;

  for (i = 0; i < hand_list_count(moves); i++) {
    int value = ai_move_value(view, table, hand_list_at(moves, i));

    if ((chosen < 0) || (value < chosenvalue)) {
      chosen = i;
      chosenvalue = value;
    }
  }

  if (chosen < 0)
    return false;

  hand_copy(hand, hand_list_at(moves, chosen));
  return true;
}

static void ai_moves_lead(const ai_view_t *view, hand_t *hand) {
  hand_list_t moves;

  move_generate(view->cards, &moves);
  ai_moves_cheapest(view, NULL, &moves, hand);
}

static bool ai_moves_beat(const ai_view_t *view, hand_t *hand) {
  hand_list_t moves;

  move_generate_beats(view->cards, view->last_hand, &moves);
  return ai_moves_cheapest(view, NULL, &moves, hand);
}

static void ai_table_lead(const ai_view_t *view, hand_t *hand) {
  hand_list_t moves;
  ai_table_t table;

  ai_table_read(view, &table);
  move_generate(view->cards, &moves);
  ai_moves_cheapest(view, &table, &moves, hand);
}

static bool ai_table_beat(const ai_view_t *view, hand_t *hand) {
  hand_list_t moves;
  ai_table_t table;

  ai_table_read(view, &table);
  move_generate_beats(view->cards, view->last_hand, &moves);
  return ai_moves_cheapest(view, &table, &moves, hand);
}

const ai_t ai_moves = {ai_moves_lead, ai_moves_beat};

const ai_t ai_table = {ai_table_lead, ai_table_beat};

void ai_lead(const ai_view_t *view, hand_t *hand) {
  hand_clear(hand);
  view->ai->lead(view, hand);
}

bool ai_beat(const ai_view_t *view, hand_t *hand) {
  bool canbeat = false;

  hand_clear(hand);

  canbeat = view->ai->beat(view, hand);

  /* peasant cooperation: the last hand came from the other peasant */
  if (canbeat && (view->seat != view->landlord) &&
      (view->last_player != view->landlord)) {
    /* don't bomb/nuke teammate */
    if (hand_is_bomb(hand) || hand_is_nuke(hand))
      canbeat = false;

    /* let the teammate run when it is closer to going out */
    if (view->cards_left[view->last_player] < view->cards_left[view->seat])
      canbeat = false;
  }

  return canbeat;
}
