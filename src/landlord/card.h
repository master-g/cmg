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

#ifndef LANDLORD_CARD_H_
#define LANDLORD_CARD_H_

#include "common.h"
#include "lmath.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CARD_RANK_3 (uint8_t)0x01
#define CARD_RANK_4 (uint8_t)0x02
#define CARD_RANK_5 (uint8_t)0x03
#define CARD_RANK_6 (uint8_t)0x04
#define CARD_RANK_7 (uint8_t)0x05
#define CARD_RANK_8 (uint8_t)0x06
#define CARD_RANK_9 (uint8_t)0x07
#define CARD_RANK_T (uint8_t)0x08
#define CARD_RANK_J (uint8_t)0x09
#define CARD_RANK_Q (uint8_t)0x0A
#define CARD_RANK_K (uint8_t)0x0B
#define CARD_RANK_A (uint8_t)0x0C
#define CARD_RANK_2 (uint8_t)0x0D
#define CARD_RANK_r (uint8_t)0x0E
#define CARD_RANK_R (uint8_t)0x0F

#define CARD_RANK_BEG CARD_RANK_3
#define CARD_RANK_END (CARD_RANK_R + 1)

#define CARD_SUIT_CLUB 0x10
#define CARD_SUIT_DIAMOND 0x20
#define CARD_SUIT_HEART 0x30
#define CARD_SUIT_SPADE 0x40

#define CARD_SET_LENGTH 54

#define CARD_RANK(x) (uint8_t)((x) & 0x0F)
#define CARD_SUIT(x) ((x) & 0xF0)

/*
 * ************************************************************
 * card array
 *
 * An ordered bunch of cards, at most one deck. Callers ask for cards by rank
 * and never have to keep the array sorted or do index arithmetic themselves;
 * the fields are for this module only.
 * ************************************************************
 */

typedef struct card_arr_s {
  int length;
  uint8_t cards[CARD_SET_LENGTH];
} card_array_t;

#define CardArray_Clear(a) (memset((a), 0, sizeof(card_array_t)))
#define CardArray_Copy(d, s) (memcpy((d), (s), sizeof(card_array_t)))
#define CardArray_IsFull(a) (CardArray_Length(a) >= CARD_SET_LENGTH)
#define CardArray_IsEmpty(a) (CardArray_Length(a) == 0)

/*
 * how many cards
 */
int CardArray_Length(const card_array_t *array);

/*
 * the card at position i, 0 when there is none
 */
uint8_t CardArray_At(const card_array_t *array, int i);

/*
 * Fill a card array from a string such as "♠A ♥T ♣3 ♦r"
 * (or "sA hT c3 dr" without LL_GRAPHICAL_SUIT).
 * A card is a suit and a rank in either order, anything else is skipped.
 */
void CardArray_InitFromString(card_array_t *array, const char *str);

/*
 * Reset a card array to the 54 cards of a deck, in a fixed order
 */
void CardArray_Reset(card_array_t *array);

/*
 * Fisher-Yates shuffle
 */
void CardArray_Shuffle(card_array_t *array, mt19937_t *mt);

/*
 * move up to count cards from the back of deck into array, replacing what
 * array held; returns how many were dealt
 */
int CardArray_Deal(card_array_t *deck, card_array_t *array, int count);

/*
 * append tail to head as far as it fits, returns how many cards were appended
 */
int CardArray_Concat(card_array_t *head, const card_array_t *tail);

/*
 * remove every card of sub from from
 */
void CardArray_Subtract(card_array_t *from, const card_array_t *sub);

/*
 * is every card of segment in array
 */
int CardArray_IsContain(const card_array_t *array, const card_array_t *segment);

/*
 * push a card to the rear of the array, ignored when the array is full
 */
void CardArray_PushBack(card_array_t *array, uint8_t card);

/*
 * pop a card from the front of the array, 0 when it is empty
 */
uint8_t CardArray_PopFront(card_array_t *array);

/*
 * drop multiple cards from the front of the array, returns how many
 */
int CardArray_DropFront(card_array_t *array, int count);

/*
 * count[rank] = how many cards of that rank, for every rank up to
 * CARD_RANK_END
 */
void CardArray_CountRanks(const card_array_t *array, int *count);

/*
 * append the first count cards of a rank in src to dst, in the order src
 * holds them; returns how many were appended
 */
int CardArray_TakeRank(
    card_array_t *dst, const card_array_t *src, uint8_t rank, int count);

/*
 * append every card of a rank in src to dst
 */
void CardArray_CopyRank(
    card_array_t *dst, const card_array_t *src, uint8_t rank);

/*
 * remove specific rank cards from array
 */
void CardArray_RemoveRank(card_array_t *array, uint8_t rank);

/*
 * sort cards from high to low, by rank then by suit
 */
void CardArray_Sort(card_array_t *array);

/*
 * reverse cards
 */
void CardArray_Reverse(card_array_t *array);

/*
 * print every card in the array
 */
void CardArray_Print(const card_array_t *array);

/* a card as text needs this much room, including the terminator */
#define CARD_STRING_SIZE 5

/*
 * Write a card as text, always terminated. Returns the number of characters
 * written, 0 when buf is smaller than CARD_STRING_SIZE. A suit or rank that
 * is not a card's shows as '?'.
 */
int Card_ToString(uint8_t card, char *buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* LANDLORD_CARD_H_ */
