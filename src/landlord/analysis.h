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
 * analysis: take cards apart into the hands they will be played as
 *
 * The result covers the cards exactly, every card is in one hand. How many
 * hands it takes is how good the cards are, fewer is better.
 * ************************************************************/

/* an analysis takes cards apart */
typedef void (*analysis_func_t)(const card_array_t *cards, hand_list_t *hl);

/*
 * greedy: nuke, bombs and 2 first, then whatever chains the rest forms
 */
void analysis_standard(const card_array_t *array, hand_list_t *hl);

/*
 * searches the ways to pull chains out for the split with the fewest hands
 */
void analysis_advanced(const card_array_t *array, hand_list_t *hl);

/*
 * how many hands the cards take when taken apart by analyze
 */
int analysis_count_hands(analysis_func_t analyze, const card_array_t *array);

#endif /* LANDLORD_ANALYSIS_H_ */
