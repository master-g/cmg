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

void Game_Init(game_t *game) {
  int i = 0;

  memset(game, 0, sizeof(game_t));

  /* every game is played by the same AI setup */
  for (i = 0; i < GAME_PLAYERS; i++) {
    game->players[i].ai = &AI_Advanced;
    game->players[i].identity = PlayerIdentity_Peasant;
    game->players[i].seatId = i;
  }

  Random_Init(&game->mt, 0);
  CardArray_Reset(&game->deck);
}

void Game_Clear(game_t *game) {
  int i = 0;

  for (i = 0; i < GAME_PLAYERS; i++)
    Player_Clear(&game->players[i]);
}

void Game_Reset(game_t *game) {
  int i = 0;

  for (i = 0; i < GAME_PLAYERS; i++)
    Player_Clear(&game->players[i]);

  game->bid = 0;
  game->playerIndex = 0;
  game->landlord = 0;
  game->lastplay = 0;
  game->winner = 0;
  game->status = 0;
  game->phase = 0;

  Hand_Clear(&game->lastHand);
  CardArray_Reset(&game->deck);
  CardArray_Clear(&game->kittyCards);
  CardArray_Clear(&game->cardRecord);
}

/* what the current player is allowed to know */
static void Game_MakeView(game_t *game, ai_view_t *view, hand_t *tobeat) {
  player_t *player = Game_GetCurrentPlayer(game);
  int i = 0;

  view->ai = player->ai;
  view->seat = game->playerIndex;
  view->landlord = game->landlord;
  view->bid = game->bid;
  view->cards = &player->cards;
  view->hands = player->handlist;
  view->lastHand = tobeat;
  view->lastPlayer = game->lastplay;
  view->played = &game->cardRecord;

  for (i = 0; i < GAME_PLAYERS; i++)
    view->cardsLeft[i] = CardArray_Length(&game->players[i].cards);
}

static void Game_Reject(game_t *game, hand_t *hand, const char *reason) {
  int i = 0;
  char str[CARD_STRING_SIZE];

  fprintf(
      stderr, "seed %u: player %d, %s:", (unsigned)game->seed,
      game->playerIndex, reason);
  for (i = 0; i < CardArray_Length(&hand->cards); i++) {
    Card_ToString(CardArray_At(&hand->cards, i), str, sizeof(str));
    fprintf(stderr, " %s", str);
  }
  fprintf(stderr, "\n");

  game->status = GameStatus_Illegal;
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
static int Game_AcceptHand(game_t *game, hand_t *played, hand_t *tobeat) {
  player_t *player = Game_GetCurrentPlayer(game);
  hand_t hand;

  if (Hand_Parse(&hand, &played->cards) == HAND_NONE) {
    Game_Reject(game, played, "not a hand");
    return 0;
  }

  if ((tobeat != NULL) && (Hand_Compare(&hand, tobeat) != HAND_CMP_GREATER)) {
    Game_Reject(game, played, "does not beat the last hand");
    return 0;
  }

  if (!CardArray_IsContain(&player->cards, &hand.cards)) {
    Game_Reject(game, played, "not the player's cards");
    return 0;
  }

  CardArray_Subtract(&player->cards, &hand.cards);

  if (tobeat == NULL) {
    /* a lead is made of whole hands of the analysis, the rest still holds */
    rk_list_node_t *node = player->handlist->first;

    while (node != NULL) {
      rk_list_node_t *next = node->next;

      if (CardArray_IsContain(&hand.cards, &HandList_GetHand(node)->cards))
        free(rk_list_remove(player->handlist, node));

      node = next;
    }
  } else {
    /* a beat may break hands up, take the cards apart again */
    rk_list_clear_destroy(player->handlist);
    player->handlist = player->ai->analyze(&player->cards);
  }

  Hand_Copy(&game->lastHand, &hand);
  game->lastplay = game->playerIndex;
  game->phase = Phase_Query;
  CardArray_Concat(&game->cardRecord, &game->lastHand.cards);

  return 1;
}

void Game_Play(game_t *game, uint32_t seed) {
  int i = 0;
  int beat = 0;
  int bid = 0;
  ai_view_t view;
  hand_t played;
  hand_t tobeat;

  /* the seed alone decides the game: seed, then shuffle a fresh deck */
  game->seed = seed;
  Random_Init(&game->mt, seed);
  CardArray_Reset(&game->deck);
  CardArray_Shuffle(&game->deck, &game->mt);

  /* bid */
  /* TODO log */
  game->status = GameStatus_Bid;
  game->bid = 0;
  game->highestBidder = -1;

  while (game->status == GameStatus_Bid) {
    game->playerIndex = Random_Int32(&game->mt) % GAME_PLAYERS;

    for (i = 0; i < GAME_PLAYERS; i++) {
      CardArray_Deal(
          &game->deck, &Game_GetCurrentPlayer(game)->cards, GAME_HAND_CARDS);
      Game_MakeView(game, &view, NULL);
      bid = AI_Bid(&view);

      if (bid > game->bid) {
        DBGLog("\nPlayer ---- %d ---- bid for %d\n", game->playerIndex, bid);
        game->highestBidder = game->playerIndex;
        game->bid = bid;
      }

      Game_IncPlayerIndex(game);
    }

    /* check if bid stage is done */
    if (game->bid == 0) {
      /* nobody bid, deal again from a reshuffled deck */
      CardArray_Reset(&game->deck);
      CardArray_Shuffle(&game->deck, &game->mt);
    } else {
      /* setup landlord, game start! */
      game->landlord = game->highestBidder;
      game->players[game->landlord].identity = PlayerIdentity_Landlord;
      game->playerIndex = game->landlord;
      game->phase = Phase_Play;
      CardArray_Deal(&game->deck, &game->kittyCards, GAME_REST_CARDS);
      CardArray_Concat(&game->players[game->landlord].cards, &game->kittyCards);
      game->status = GameStatus_Ready;
    }
  }

  /* everybody sorts their cards and takes them apart */
  for (i = 0; i < GAME_PLAYERS; i++) {
    player_t *player = &game->players[i];

    CardArray_Sort(&player->cards);
    player->handlist = player->ai->analyze(&player->cards);
  }

  /* game play */
  while (game->status == GameStatus_Ready) {
    if (game->phase == Phase_Play) {
      Game_MakeView(game, &view, NULL);
      AI_Lead(&view, &played);

      if (!Game_AcceptHand(game, &played, NULL))
        break;

      DBGLog("\nPlayer ---- %d ---- played\n", game->playerIndex);
      Hand_Print(&game->lastHand);
    } else if ((game->phase == Phase_Query) || (game->phase == Phase_Pass)) {
      Hand_Copy(&tobeat, &game->lastHand);
      Game_MakeView(game, &view, &tobeat);
      beat = AI_Beat(&view, &played);

      /* has beat in this phase */
      if (beat == 0) {
        /* two player pass */
        if (game->phase == Phase_Pass)
          game->phase = Phase_Play;
        else
          game->phase = Phase_Pass;

        DBGLog("\nPlayer ---- %d ---- passed\n", game->playerIndex);
      } else {
        if (!Game_AcceptHand(game, &played, &tobeat))
          break;

        DBGLog("\nPlayer ---- %d ---- beat\n", game->playerIndex);
        Hand_Print(&game->lastHand);
      }
    }

    Game_IncPlayerIndex(game);

    /* check if there is player win */
    for (i = 0; i < GAME_PLAYERS; i++) {
      if (CardArray_IsEmpty(&game->players[i].cards)) {
        game->status = GameStatus_Over;
        game->winner = i;

        DBGLog("\nPlayer ++++ %d ++++ wins!\n", i);
        break;
      }
    }
  }
}
