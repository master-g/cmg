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

#ifndef LANDLORD_HANDLIST_H
#define LANDLORD_HANDLIST_H

#include "hand.h"
#include "ruiko_algorithm.h"

/* ************************************************************
 * hand list: a list whose payloads are hands
 * ************************************************************/

/*
 * append a copy of hand to the hand list
 */
void HandList_PushFront(rk_list_t *hl, hand_t *hand);

/*
 * get payload as hand_t
 */
#define HandList_GetHand(h) ((hand_t *)((h)->payload))

/*
 * print hand_list_t
 */
void HandList_Print(rk_list_t *hl);

#endif /* LANDLORD_HANDLIST_H */
