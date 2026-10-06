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

#ifndef LANDLORD_PLAYER_H_
#define LANDLORD_PLAYER_H_

#include "ai.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PlayerIdentity_Peasant = 0,
  PlayerIdentity_Landlord

} PlayerIdentity;

/* a seat at the table, owned and kept up to date by the game */
typedef struct player_s {
  card_array_t cards;  /* card array, will change during game play */
  rk_list_t *handlist; /* cards taken apart by the seat's AI */
  int identity;        /* 0: peasant, 1: landlord */
  int seatId;          /* 0, 1, 2 */
  const ai_t *ai;      /* who decides for this seat */

} player_t;

/*
 * clear a player context
 */
void Player_Clear(player_t *player);

#ifdef __cplusplus
}
#endif

#endif /* LANDLORD_PLAYER_H_ */
