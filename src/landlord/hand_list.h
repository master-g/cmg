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

#ifndef LANDLORD_HAND_LIST_H_
#define LANDLORD_HAND_LIST_H_

#include "hand.h"

/* ************************************************************
 * hand list: an ordered list of hands, held by value
 *
 * 256 is above anything one seat's 20 cards can produce, the most being the
 * ways to pick three kickers for a three trio chain. A full list refuses
 * further hands, it never overflows.
 * ************************************************************/

#define HAND_LIST_CAPACITY 256

typedef struct hand_list_s {
  int count;
  hand_t hands[HAND_LIST_CAPACITY];
} hand_list_t;

/*
 * empty the list
 */
void hand_list_clear(hand_list_t *hl);

/*
 * how many hands
 */
int hand_list_count(const hand_list_t *hl);

/*
 * the hand at position i, NULL when there is none
 */
const hand_t *hand_list_at(const hand_list_t *hl, int i);

/*
 * append a copy of hand, returns false and leaves the list alone when it is
 * full
 */
bool hand_list_push(hand_list_t *hl, const hand_t *hand);

/*
 * remove every hand made only of cards in `cards`, keeping the order of the
 * rest
 */
void hand_list_remove_contained(hand_list_t *hl, const card_array_t *cards);

/*
 * print hand_list_t
 */
void hand_list_print(const hand_list_t *hl);

#endif /* LANDLORD_HAND_LIST_H_ */
