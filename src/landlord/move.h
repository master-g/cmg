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

#ifndef LANDLORD_MOVE_H_
#define LANDLORD_MOVE_H_

#include "hand.h"
#include "hand_list.h"

/*
 * Every hand that can be led from cards, as the rules judge them. Which
 * suits a hand takes never matters, so each one comes once.
 *
 * A single trio comes with every kicker it could carry. A trio chain or a
 * four comes with one choice of kickers only: the ranks held the fewest
 * times, lowest first. Chains of fours are not generated.
 */
void move_generate(const card_array_t *cards, hand_list_t *moves);

/*
 * the moves from cards that beat tobeat
 */
void move_generate_beats(
    const card_array_t *cards, const hand_t *tobeat, hand_list_t *moves);

#endif /* LANDLORD_MOVE_H_ */
