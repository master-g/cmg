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

#ifndef LANDLORD_ANALYSIS_H
#define LANDLORD_ANALYSIS_H

#include "hand.h"
#include "handlist.h"

/* ************************************************************
 * analysis: take cards apart into the hands they will be played as
 *
 * The result covers the cards exactly, every card is in one hand. How many
 * hands it takes is how good the cards are, fewer is better.
 * The caller destroys the list with rk_list_clear_destroy.
 * ************************************************************/

/* an analysis takes cards apart */
typedef rk_list_t *(*Analysis_Func)(card_array_t *cards);

/*
 * greedy: nuke, bombs and 2 first, then whatever chains the rest forms
 */
rk_list_t *Analysis_Standard(card_array_t *array);

/*
 * searches the ways to pull chains out for the split with the fewest hands
 */
rk_list_t *Analysis_Advanced(card_array_t *array);

/*
 * how many hands the cards take when taken apart by analyze
 */
int Analysis_CountHands(Analysis_Func analyze, card_array_t *array);

#endif /* LANDLORD_ANALYSIS_H */
