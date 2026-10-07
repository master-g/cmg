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

#ifndef LANDLORD_ANALYSIS_H_
#define LANDLORD_ANALYSIS_H_

#include "hand.h"
#include "hand_list.h"

/* ************************************************************
 * analysis: how many hands cards will take to play, fewer is better
 * ************************************************************/

/*
 * Greedy: nuke, bombs and 2 first, then whatever chains the rest forms. The
 * hands cover the cards exactly, every card is in one of them.
 */
void analysis_standard(const card_array_t *array, hand_list_t *hl);

/*
 * The fewest turns the cards can be played in. Searches the ways to pull
 * chains out, on the rank counts alone; a trio and the kicker it carries are
 * one turn. Nuke, bombs and 2 are never broken up.
 */
int analysis_counted_hands(const card_array_t *array);

/*
 * The split analysis_counted_hands counted: the hands that search would play
 * the cards as. A kicker is still a hand of its own here, next to the trio
 * that will carry it, so the list is longer than the number of turns.
 */
void analysis_counted(const card_array_t *array, hand_list_t *hl);

#endif /* LANDLORD_ANALYSIS_H_ */
