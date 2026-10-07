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

#ifndef LANDLORD_AI_H_
#define LANDLORD_AI_H_

#include "analysis.h"
#include "hand.h"
#include "move.h"

/* seats at the table */
#define GAME_PLAYERS 3

/*
 * An AI only decides: it is shown what its seat may know and answers with a
 * bid or a hand. It never changes the game, the game applies the decision.
 *
 * http://scim.brad.ac.uk/staff/pdf/picowlin/AISB2011.pdf
 * http://en.wikipedia.org/wiki/Monte-Carlo_tree_search
 * http://mcts.ai/about/index.html
 */

struct ai_view_s;

/* what the AIs differ in: how one picks the hand to lead or to beat with */
typedef struct ai_s {
  void (*lead)(const struct ai_view_s *view, hand_t *hand);
  bool (*beat)(const struct ai_view_s *view, hand_t *hand);
} ai_t;

/* weighs every legal move by the turns the cards left behind would take */
extern const ai_t ai_moves;

/* the same, and reads the table: cards left, cards played, who sides with whom
 */
extern const ai_t ai_table;

/* what one seat may know, all of it read only */
typedef struct ai_view_s {
  const ai_t *ai;
  int seat;                     /* the seat that has to decide */
  int landlord;                 /* landlord's seat, not known while bidding */
  int bid;                      /* highest bid so far */
  const card_array_t *cards;    /* the seat's own cards */
  const hand_t *last_hand;      /* the hand to beat, NULL when leading */
  int last_player;              /* who played last_hand */
  int cards_left[GAME_PLAYERS]; /* cards every seat still holds */
  const card_array_t *played;   /* every card played so far */
} ai_view_t;

/*
 * how much to bid for landlord, 0 to stay out
 */
int ai_bid(const ai_view_t *view);

/*
 * choose the hand to lead with, always plays
 */
void ai_lead(const ai_view_t *view, hand_t *hand);

/*
 * choose a hand that beats view->last_hand
 * returns false to pass, in which case hand is not meaningful
 */
bool ai_beat(const ai_view_t *view, hand_t *hand);

#endif /* LANDLORD_AI_H_ */
