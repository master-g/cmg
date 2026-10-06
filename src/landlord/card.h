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

#include <stdbool.h>

/* a card is one byte: suit in the high half, rank in the low half */

typedef enum {
  CARD_RANK_NONE = 0,
  CARD_RANK_3,
  CARD_RANK_4,
  CARD_RANK_5,
  CARD_RANK_6,
  CARD_RANK_7,
  CARD_RANK_8,
  CARD_RANK_9,
  CARD_RANK_T,
  CARD_RANK_J,
  CARD_RANK_Q,
  CARD_RANK_K,
  CARD_RANK_A,
  CARD_RANK_2,
  CARD_RANK_BLACK_JOKER, /* black joker */
  CARD_RANK_RED_JOKER,   /* red joker */

  CARD_RANK_END,
  CARD_RANK_BEG = CARD_RANK_3
} card_rank_t;

typedef enum {
  CARD_SUIT_NONE = 0x00,
  CARD_SUIT_CLUB = 0x10,
  CARD_SUIT_DIAMOND = 0x20,
  CARD_SUIT_HEART = 0x30,
  CARD_SUIT_SPADE = 0x40
} card_suit_t;

#define CARD_SET_LENGTH 54

/* a card_rank_t, as an int since ranks are counted and compared */
#define CARD_RANK(x) ((int)((x) & 0x0F))
#define CARD_SUIT(x) ((card_suit_t)((x) & 0xF0))

/*
 * ************************************************************
 * card array
 *
 * An ordered bunch of cards, at most one deck. Callers ask for cards by rank
 * and never have to keep the array sorted or do index arithmetic themselves;
 * the fields are for this module only.
 * ************************************************************
 */

typedef struct card_array_s {
  int length;
  uint8_t cards[CARD_SET_LENGTH];
} card_array_t;

/*
 * empty the array
 */
void card_array_clear(card_array_t *array);

/*
 * make dst hold the same cards in the same order as src
 */
void card_array_copy(card_array_t *dst, const card_array_t *src);

/*
 * does it hold a whole deck, so nothing more fits
 */
bool card_array_is_full(const card_array_t *array);

/*
 * does it hold no card
 */
bool card_array_is_empty(const card_array_t *array);

/*
 * how many cards
 */
int card_array_length(const card_array_t *array);

/*
 * the card at position i, 0 when there is none
 */
uint8_t card_array_at(const card_array_t *array, int i);

/*
 * Fill a card array from a string such as "♠A ♥T ♣3 ♦r"
 * (or "sA hT c3 dr" without LANDLORD_GRAPHICAL_SUIT).
 * A card is a suit and a rank in either order, anything else is skipped.
 */
void card_array_init_from_string(card_array_t *array, const char *str);

/*
 * Reset a card array to the 54 cards of a deck, in a fixed order
 */
void card_array_reset(card_array_t *array);

/*
 * Fisher-Yates shuffle
 */
void card_array_shuffle(card_array_t *array, mt19937_t *mt);

/*
 * move up to count cards from the back of deck into array, replacing what
 * array held; returns how many were dealt
 */
int card_array_deal(card_array_t *deck, card_array_t *array, int count);

/*
 * append tail to head as far as it fits, returns how many cards were appended
 */
int card_array_concat(card_array_t *head, const card_array_t *tail);

/*
 * remove every card of sub from from
 */
void card_array_subtract(card_array_t *from, const card_array_t *sub);

/*
 * is every card of segment in array
 */
bool card_array_contains(
    const card_array_t *array, const card_array_t *segment);

/*
 * push a card to the rear of the array, ignored when the array is full
 */
void card_array_push_back(card_array_t *array, uint8_t card);

/*
 * pop a card from the front of the array, 0 when it is empty
 */
uint8_t card_array_pop_front(card_array_t *array);

/*
 * drop multiple cards from the front of the array, returns how many
 */
int card_array_drop_front(card_array_t *array, int count);

/*
 * count[rank] = how many cards of that rank, for every rank up to
 * CARD_RANK_END
 */
void card_array_count_ranks(const card_array_t *array, int *count);

/*
 * append the first count cards of a rank in src to dst, in the order src
 * holds them; returns how many were appended
 */
int card_array_take_rank(
    card_array_t *dst, const card_array_t *src, int rank, int count);

/*
 * append every card of a rank in src to dst
 */
void card_array_copy_rank(card_array_t *dst, const card_array_t *src, int rank);

/*
 * remove specific rank cards from array
 */
void card_array_remove_rank(card_array_t *array, int rank);

/*
 * sort cards from high to low, by rank then by suit
 */
void card_array_sort(card_array_t *array);

/*
 * reverse cards
 */
void card_array_reverse(card_array_t *array);

/*
 * print every card in the array
 */
void card_array_print(const card_array_t *array);

/* a card as text needs this much room, including the terminator */
#define CARD_STRING_SIZE 5

/*
 * Write a card as text, always terminated. Returns the number of characters
 * written, 0 when buf is smaller than CARD_STRING_SIZE. A suit or rank that
 * is not a card's shows as '?'.
 */
int card_to_string(uint8_t card, char *buf, int len);

#endif /* LANDLORD_CARD_H_ */
