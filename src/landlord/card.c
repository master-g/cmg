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

#include "card.h"

static const uint8_t card_set[] = {
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
    0x1C, 0x1D, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    0x2A, 0x2B, 0x2C, 0x2D, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x41, 0x42, 0x43, 0x44, 0x45,
    0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x1E, 0x2F};

/* ************************************************************
 * text
 * ************************************************************/

#ifdef LANDLORD_GRAPHICAL_SUIT
#define CARD_SUIT_STRING_LENGTH 3

static const char suit_text_diamond[] = "\xE2\x99\xA6";
static const char suit_text_club[] = "\xE2\x99\xA3";
static const char suit_text_heart[] = "\xE2\x99\xA5";
static const char suit_text_spade[] = "\xE2\x99\xA0";
#else /* ifdef LANDLORD_GRAPHICAL_SUIT */
#define CARD_SUIT_STRING_LENGTH 1

static const char suit_text_diamond[] = "d";
static const char suit_text_club[] = "c";
static const char suit_text_heart[] = "h";
static const char suit_text_spade[] = "s";
#endif /* ifdef LANDLORD_GRAPHICAL_SUIT */

/* indexed by rank, rank 0 is no card */
static const char rank_text[] = "?3456789TJQKA2rR";

int card_to_string(uint8_t card, char *buf, int len) {
  const char *suit_text = NULL;

  if ((buf == NULL) || (len < CARD_STRING_SIZE))
    return 0;

  /* the byte may hold a suit no card has, hence the default */
  switch (CARD_SUIT(card)) {
  case CARD_SUIT_DIAMOND:
    suit_text = suit_text_diamond;
    break;

  case CARD_SUIT_CLUB:
    suit_text = suit_text_club;
    break;

  case CARD_SUIT_HEART:
    suit_text = suit_text_heart;
    break;

  case CARD_SUIT_SPADE:
    suit_text = suit_text_spade;
    break;

  case CARD_SUIT_NONE:
  default:
    suit_text = NULL;
    break;
  }

  if (suit_text == NULL) {
    buf[0] = '?';
    buf[1] = rank_text[CARD_RANK(card)];
    buf[2] = '\0';
    return 2;
  }

  memcpy(buf, suit_text, CARD_SUIT_STRING_LENGTH);
  buf[CARD_SUIT_STRING_LENGTH] = rank_text[CARD_RANK(card)];
  buf[CARD_SUIT_STRING_LENGTH + 1] = '\0';

  return CARD_SUIT_STRING_LENGTH + 1;
}

/* the suit str starts with, 0 when it does not start with one */
static card_suit_t card_parse_suit(const char *str, int *consumed) {
  static const struct {
    const char *text;
    card_suit_t suit;
  } suits[] = {
      {suit_text_diamond, CARD_SUIT_DIAMOND},
      {suit_text_club, CARD_SUIT_CLUB},
      {suit_text_heart, CARD_SUIT_HEART},
      {suit_text_spade, CARD_SUIT_SPADE}};
  size_t i = 0;

  for (i = 0; i < sizeof(suits) / sizeof(suits[0]); i++) {
    if (strncmp(str, suits[i].text, CARD_SUIT_STRING_LENGTH) == 0) {
      *consumed = CARD_SUIT_STRING_LENGTH;
      return suits[i].suit;
    }
  }

  *consumed = 1;
  return CARD_SUIT_NONE;
}

/* the rank a character stands for, 0 when it is not a rank */
static int card_parse_rank(char c) {
  const char *found = (c != '\0' && c != '?') ? strchr(rank_text, c) : NULL;

  return found != NULL ? (int)(found - rank_text) : CARD_RANK_NONE;
}

void card_array_init_from_string(card_array_t *array, const char *str) {
  card_suit_t suit = CARD_SUIT_NONE;
  int rank = CARD_RANK_NONE;
  const char *p = str;

  card_array_clear(array);

  while (*p != '\0') {
    int consumed = 1;
    card_suit_t s = card_parse_suit(p, &consumed);

    if (s != CARD_SUIT_NONE)
      suit = s;
    else if (card_parse_rank(*p) != CARD_RANK_NONE)
      rank = card_parse_rank(*p);

    if ((suit != CARD_SUIT_NONE) && (rank != CARD_RANK_NONE)) {
      card_array_push_back(array, (uint8_t)((int)suit | rank));
      suit = CARD_SUIT_NONE;
      rank = CARD_RANK_NONE;
    }

    p += consumed;
  }
}

void card_array_print(const card_array_t *array) {
#if (LANDLORD_PRINT_LOG == 1)
  int i = 0;
  char str[CARD_STRING_SIZE];

  LANDLORD_LOG("Cards: (%d): ", array->length);

  for (i = 0; i < array->length; i++) {
    card_to_string(array->cards[i], str, sizeof(str));
    LANDLORD_LOG("%s ", str);
  }

  LANDLORD_LOG("\n");
#else
  (void)array;
#endif
}

/* ************************************************************
 * card array
 * ************************************************************/

void card_array_clear(card_array_t *array) {
  memset(array, 0, sizeof(card_array_t));
}

void card_array_copy(card_array_t *dst, const card_array_t *src) {
  memcpy(dst, src, sizeof(card_array_t));
}

bool card_array_is_full(const card_array_t *array) {
  return array->length >= CARD_SET_LENGTH;
}

bool card_array_is_empty(const card_array_t *array) {
  return array->length == 0;
}

int card_array_length(const card_array_t *array) { return array->length; }

uint8_t card_array_at(const card_array_t *array, int i) {
  return ((i >= 0) && (i < array->length)) ? array->cards[i] : 0;
}

void card_array_reset(card_array_t *array) {
  memcpy(array->cards, card_set, sizeof(uint8_t) * CARD_SET_LENGTH);
  array->length = CARD_SET_LENGTH;
}

void card_array_shuffle(card_array_t *array, mt19937_t *mt) {
  int i = array->length;
  int j = 0;
  uint8_t tmp = 0;

  while (--i > 0) {
    j = mt19937_int32(mt) % (i + 1);

    tmp = array->cards[j];
    array->cards[j] = array->cards[i];
    array->cards[i] = tmp;
  }
}

int card_array_deal(card_array_t *deck, card_array_t *array, int count) {
  int dealt = 0;

  card_array_clear(array);

  dealt = deck->length >= count ? count : deck->length;

  deck->length -= dealt;
  memcpy(array->cards, &deck->cards[deck->length], (size_t)dealt);
  array->length = dealt;

  return dealt;
}

int card_array_concat(card_array_t *head, const card_array_t *tail) {
  int slot = CARD_SET_LENGTH - head->length;
  int length = slot >= tail->length ? tail->length : slot;

  memcpy(&head->cards[head->length], tail->cards, (size_t)length);
  head->length += length;

  return length;
}

void card_array_subtract(card_array_t *from, const card_array_t *sub) {
  int i = 0;
  int j = 0;
  uint8_t card = 0;
  card_array_t temp;

  card_array_clear(&temp);

  for (i = 0; i < from->length; i++) {
    card = from->cards[i];

    for (j = 0; j < sub->length; j++) {
      if (card == sub->cards[j]) {
        card = 0;
        break;
      }
    }

    if (card != 0)
      card_array_push_back(&temp, card);
  }

  card_array_copy(from, &temp);
}

bool card_array_contains(
    const card_array_t *array, const card_array_t *segment) {
  int i = 0;
  int j = 0;
  card_array_t temp;

  if ((array->length == 0) || (segment->length == 0) ||
      (array->length < segment->length))
    return false;

  /* cross every card of the segment off as it is found */
  card_array_copy(&temp, segment);

  for (i = 0; i < array->length; i++) {
    for (j = 0; j < temp.length; j++) {
      if (array->cards[i] == temp.cards[j]) {
        temp.cards[j] = temp.cards[--temp.length];
        break;
      }
    }
  }

  return temp.length == 0;
}

void card_array_push_back(card_array_t *array, uint8_t card) {
  if (array->length < CARD_SET_LENGTH)
    array->cards[array->length++] = card;
}

uint8_t card_array_pop_front(card_array_t *array) {
  uint8_t card = 0;

  if (array->length > 0) {
    card = array->cards[0];
    array->length--;
    memmove(array->cards, array->cards + 1, (size_t)array->length);
    array->cards[array->length] = 0;
  }

  return card;
}

int card_array_drop_front(card_array_t *array, int count) {
  int drop = (array->length >= count) ? count : array->length;

  array->length -= drop;
  memmove(array->cards, array->cards + drop, (size_t)array->length);
  memset(array->cards + array->length, 0, (size_t)drop);

  return drop;
}

void card_array_count_ranks(const card_array_t *array, int *count) {
  int i = 0;

  memset(count, 0, sizeof(int) * CARD_RANK_END);

  for (i = 0; i < array->length; i++)
    count[CARD_RANK(array->cards[i])]++;
}

int card_array_take_rank(
    card_array_t *dst, const card_array_t *src, int rank, int count) {
  int i = 0;
  int taken = 0;

  for (i = 0; (i < src->length) && (taken < count); i++) {
    if (CARD_RANK(src->cards[i]) == rank) {
      card_array_push_back(dst, src->cards[i]);
      taken++;
    }
  }

  return taken;
}

void card_array_copy_rank(
    card_array_t *dst, const card_array_t *src, int rank) {
  card_array_take_rank(dst, src, rank, src->length);
}

void card_array_remove_rank(card_array_t *array, int rank) {
  int i = 0;
  card_array_t temp;

  card_array_clear(&temp);

  for (i = 0; i < array->length; i++) {
    if (CARD_RANK(array->cards[i]) != rank)
      card_array_push_back(&temp, array->cards[i]);
  }

  card_array_copy(array, &temp);
}

/* high rank first, then high suit first */
static int card_array_compare(const void *a, const void *b) {
  int ra = 0;
  int rb = 0;

  /* rotation */
  ra = (int)CARD_SUIT(*(const uint8_t *)a) >> 4 | CARD_RANK(*(const uint8_t *)a)
                                                      << 4;
  rb = (int)CARD_SUIT(*(const uint8_t *)b) >> 4 | CARD_RANK(*(const uint8_t *)b)
                                                      << 4;

  return rb - ra;
}

void card_array_sort(card_array_t *array) {
  qsort(
      array->cards, (size_t)array->length, sizeof(uint8_t), card_array_compare);
}

void card_array_reverse(card_array_t *array) {
  int i, j;
  uint8_t tmp;
  for (i = 0, j = array->length - 1; i < array->length / 2; i++, j--) {
    tmp = array->cards[i];
    array->cards[i] = array->cards[j];
    array->cards[j] = tmp;
  }
}
