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

#ifndef LANDLORD_BEAT_H_
#define LANDLORD_BEAT_H_

#include "hand.h"
#include "hand_list.h"

/* ************************************************************
 * beat search: the hands in some cards that beat a given hand
 * ************************************************************/

/*
 * search for one beat, result will be stored in beat
 * 1, if [beat->type] != 0, then search [new beat] > [beat]
 * 2, search [beat] > [tobeat], then store in [beat]
 *
 * returns 0 when there is none
 */
int beat_search(const card_array_t *cards, const hand_t *tobeat, hand_t *beat);

/*
 * every hand in cards that beats tobeat, as judged by the rules, in the order
 * the search finds them
 */
void beat_search_all(
    const card_array_t *cards, const hand_t *tobeat, hand_list_t *hl);

#endif /* LANDLORD_BEAT_H_ */
