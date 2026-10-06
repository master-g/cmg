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

#include "handlist.h"

void HandList_Clear(hand_list_t *hl) { hl->count = 0; }

int HandList_Count(const hand_list_t *hl) { return hl->count; }

const hand_t *HandList_At(const hand_list_t *hl, int i) {
  return ((i >= 0) && (i < hl->count)) ? &hl->hands[i] : NULL;
}

int HandList_Push(hand_list_t *hl, const hand_t *hand) {
  if (hl->count >= HAND_LIST_CAPACITY)
    return 0;

  Hand_Copy(&hl->hands[hl->count++], hand);
  return 1;
}

void HandList_RemoveContained(hand_list_t *hl, const card_array_t *cards) {
  int i = 0;
  int kept = 0;

  for (i = 0; i < hl->count; i++) {
    if (CardArray_IsContain(cards, &hl->hands[i].cards))
      continue;

    if (kept != i)
      Hand_Copy(&hl->hands[kept], &hl->hands[i]);

    kept++;
  }

  hl->count = kept;
}

void HandList_Print(const hand_list_t *hl) {
  int i = 0;

  DBGLog("-----hand_list_t begin---------\n");
  for (i = 0; i < hl->count; i++)
    Hand_Print(&hl->hands[i]);
  DBGLog("-----hand_list_t ended---------\n");
}
