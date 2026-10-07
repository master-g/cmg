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

#ifndef LANDLORD_GAME_H_
#define LANDLORD_GAME_H_

#include "card.h"
#include "hand.h"
#include "hand_list.h"
#include "lmath.h"
#include "player.h"

#define GAME_HAND_CARDS 17
#define GAME_REST_CARDS 3

typedef enum {
  GAME_STATUS_HALT = 0,
  GAME_STATUS_BID,
  GAME_STATUS_READY,
  GAME_STATUS_OVER,
  GAME_STATUS_ILLEGAL /* a player handed in cards the rules reject */

} game_status_t;

typedef enum {
  GAME_PHASE_PLAY = 0,
  GAME_PHASE_QUERY,
  GAME_PHASE_PASS

} game_phase_t;

typedef struct game_s {
  player_t players[GAME_PLAYERS]; /* player array */
  card_array_t deck;              /* deck */
  mt19937_t mt;                   /* random context */
  hand_t last_hand;               /* last played hand */
  card_array_t card_record;       /* card record */
  card_array_t kitty_cards;       /* kitty cards */
  int bid;                        /* current bid */
  int highest_bidder;             /* for the highest bidder! */
  int player_index;               /* current player index */
  int landlord;                   /* landlord index */
  int last_play;                  /* who played the last hand */
  int winner;                     /* who win the last game */
  game_status_t status;           /* game status */
  game_phase_t phase;             /* game phase */
  uint32_t seed;                  /* seed of the game being played */

} game_t;

/*
 * seat three AIs at the table, once
 */
void game_init(game_t *game);

/*
 * Play one whole game. The seed alone decides it: the same seed gives the
 * same game whatever was played before. Afterwards the game holds the
 * result: status, winner, landlord, bid and every card played in order.
 */
void game_play(game_t *game, uint32_t seed);

#endif /* LANDLORD_GAME_H_ */
