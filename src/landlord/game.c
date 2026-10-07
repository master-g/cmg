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

#include "game.h"
#include "log.h"

#include <stdio.h>
#include <string.h>

void game_init(game_t *game) {
  int i = 0;

  memset(game, 0, sizeof(game_t));

  /* every game is played by the same AI setup */
  for (i = 0; i < GAME_PLAYERS; i++) {
    game->players[i].ai = &ai_moves;
    game->players[i].identity = PLAYER_IDENTITY_PEASANT;
    game->players[i].seat = i;
  }

  mt19937_init(&game->mt, 0);
  card_array_reset(&game->deck);
}

/* the player whose turn it is */
static player_t *game_current_player(game_t *game) {
  return &game->players[game->player_index];
}

/* the turn passes to the next seat */
static void game_next_player(game_t *game) {
  game->player_index = (game->player_index + 1) % GAME_PLAYERS;
}

/* back to an empty table, the seats keep their AI */
static void game_reset(game_t *game) {
  int i = 0;

  for (i = 0; i < GAME_PLAYERS; i++)
    player_clear(&game->players[i]);

  game->bid = 0;
  game->player_index = 0;
  game->landlord = 0;
  game->last_play = 0;
  game->winner = 0;
  game->status = GAME_STATUS_HALT;
  game->phase = GAME_PHASE_PLAY;

  hand_clear(&game->last_hand);
  card_array_reset(&game->deck);
  card_array_clear(&game->kitty_cards);
  card_array_clear(&game->card_record);
}

/* what the current player is allowed to know */
static void
game_make_view(const game_t *game, ai_view_t *view, const hand_t *tobeat) {
  const player_t *player = &game->players[game->player_index];
  int i = 0;

  view->ai = player->ai;
  view->seat = game->player_index;
  view->landlord = game->landlord;
  view->bid = game->bid;
  view->cards = &player->cards;
  view->hands = &player->hands;
  view->last_hand = tobeat;
  view->last_player = game->last_play;
  view->played = &game->card_record;

  for (i = 0; i < GAME_PLAYERS; i++)
    view->cards_left[i] = card_array_length(&game->players[i].cards);
}

static void game_reject(game_t *game, const hand_t *hand, const char *reason) {
  int i = 0;
  char str[CARD_STRING_SIZE];

  fprintf(
      stderr, "seed %u: player %d, %s:", (unsigned)game->seed,
      game->player_index, reason);
  for (i = 0; i < card_array_length(&hand->cards); i++) {
    card_to_string(card_array_at(&hand->cards, i), str, sizeof(str));
    fprintf(stderr, " %s", str);
  }
  fprintf(stderr, "\n");

  game->status = GAME_STATUS_ILLEGAL;
}

/*
 * Every hand enters the game here. The cards the current player decided on
 * must be a hand by the rules, must be greater than `tobeat` when there is
 * one, and must be cards the player holds. The hand type is the one the rules
 * give it, whatever the player labelled it.
 *
 * An accepted hand is taken out of the player's cards, becomes the last hand
 * and goes on record.
 */
static int
game_accept_hand(game_t *game, const hand_t *played, const hand_t *tobeat) {
  player_t *player = game_current_player(game);
  hand_t hand;

  if (!hand_parse(&hand, &played->cards)) {
    game_reject(game, played, "not a hand");
    return 0;
  }

  if ((tobeat != NULL) && (hand_compare(&hand, tobeat) != HAND_CMP_GREATER)) {
    game_reject(game, played, "does not beat the last hand");
    return 0;
  }

  if (!card_array_contains(&player->cards, &hand.cards)) {
    game_reject(game, played, "not the player's cards");
    return 0;
  }

  card_array_subtract(&player->cards, &hand.cards);

  if (tobeat == NULL) {
    /* a lead is made of whole hands of the analysis, the rest still holds */
    hand_list_remove_contained(&player->hands, &hand.cards);
  } else {
    /* a beat may break hands up, take the cards apart again */
    player->ai->analyze(&player->cards, &player->hands);
  }

  hand_copy(&game->last_hand, &hand);
  game->last_play = game->player_index;
  game->phase = GAME_PHASE_QUERY;
  card_array_concat(&game->card_record, &game->last_hand.cards);

  return 1;
}

void game_play(game_t *game, uint32_t seed) {
  int i = 0;
  int beat = 0;
  int bid = 0;
  ai_view_t view;
  hand_t played;
  hand_t tobeat;

  /* the seed alone decides the game: seed, then shuffle a fresh deck */
  game_reset(game);

  game->seed = seed;
  mt19937_init(&game->mt, seed);
  card_array_reset(&game->deck);
  card_array_shuffle(&game->deck, &game->mt);

  /* bid */
  /* TODO log */
  game->status = GAME_STATUS_BID;
  game->bid = 0;
  game->highest_bidder = -1;

  while (game->status == GAME_STATUS_BID) {
    game->player_index = mt19937_int32(&game->mt) % GAME_PLAYERS;

    for (i = 0; i < GAME_PLAYERS; i++) {
      card_array_deal(
          &game->deck, &game_current_player(game)->cards, GAME_HAND_CARDS);
      game_make_view(game, &view, NULL);
      bid = ai_bid(&view);

      if (bid > game->bid) {
        LANDLORD_LOG(
            "\nPlayer ---- %d ---- bid for %d\n", game->player_index, bid);
        game->highest_bidder = game->player_index;
        game->bid = bid;
      }

      game_next_player(game);
    }

    /* check if bid stage is done */
    if (game->bid == 0) {
      /* nobody bid, deal again from a reshuffled deck */
      card_array_reset(&game->deck);
      card_array_shuffle(&game->deck, &game->mt);
    } else {
      /* setup landlord, game start! */
      game->landlord = game->highest_bidder;
      game->players[game->landlord].identity = PLAYER_IDENTITY_LANDLORD;
      game->player_index = game->landlord;
      game->phase = GAME_PHASE_PLAY;
      card_array_deal(&game->deck, &game->kitty_cards, GAME_REST_CARDS);
      card_array_concat(
          &game->players[game->landlord].cards, &game->kitty_cards);
      game->status = GAME_STATUS_READY;
    }
  }

  /* everybody sorts their cards and takes them apart */
  for (i = 0; i < GAME_PLAYERS; i++) {
    player_t *player = &game->players[i];

    card_array_sort(&player->cards);
    player->ai->analyze(&player->cards, &player->hands);
  }

  /* game play */
  while (game->status == GAME_STATUS_READY) {
    if (game->phase == GAME_PHASE_PLAY) {
      game_make_view(game, &view, NULL);
      ai_lead(&view, &played);

      if (!game_accept_hand(game, &played, NULL))
        break;

      LANDLORD_LOG("\nPlayer ---- %d ---- played\n", game->player_index);
      hand_print(&game->last_hand);
    } else if (
        (game->phase == GAME_PHASE_QUERY) || (game->phase == GAME_PHASE_PASS)) {
      hand_copy(&tobeat, &game->last_hand);
      game_make_view(game, &view, &tobeat);
      beat = ai_beat(&view, &played);

      /* has beat in this phase */
      if (beat == 0) {
        /* two player pass */
        if (game->phase == GAME_PHASE_PASS)
          game->phase = GAME_PHASE_PLAY;
        else
          game->phase = GAME_PHASE_PASS;

        LANDLORD_LOG("\nPlayer ---- %d ---- passed\n", game->player_index);
      } else {
        if (!game_accept_hand(game, &played, &tobeat))
          break;

        LANDLORD_LOG("\nPlayer ---- %d ---- beat\n", game->player_index);
        hand_print(&game->last_hand);
      }
    }

    game_next_player(game);

    /* check if there is player win */
    for (i = 0; i < GAME_PLAYERS; i++) {
      if (card_array_is_empty(&game->players[i].cards)) {
        game->status = GAME_STATUS_OVER;
        game->winner = i;

        LANDLORD_LOG("\nPlayer ++++ %d ++++ wins!\n", i);
        break;
      }
    }
  }
}
