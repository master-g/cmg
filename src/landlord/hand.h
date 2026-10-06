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

#ifndef LANDLORD_HAND_H_
#define LANDLORD_HAND_H_

#include "card.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HAND_MIN_LENGTH 1
#define HAND_MAX_LENGTH 20
#define HAND_SOLO_CHAIN_MIN_LENGTH 5
#define HAND_PAIR_CHAIN_MIN_LENGTH 6
#define HAND_TRIO_CHAIN_MIN_LENGTH 6
#define HAND_FOUR_CHAIN_MIN_LENGTH 8

/*
 * What gives a hand its rank. SOLO to FOUR are also the number of cards of
 * each rank the part is made of, and the order is the order bombs beat by.
 */
typedef enum {
  HAND_PRIMAL_NONE = 0, /* not a hand */
  HAND_PRIMAL_SOLO = 1,
  HAND_PRIMAL_PAIR = 2,
  HAND_PRIMAL_TRIO = 3,
  HAND_PRIMAL_FOUR = 4, /* four of a kind carrying kickers, not a bomb */
  HAND_PRIMAL_BOMB,
  HAND_PRIMAL_NUKE
} hand_primal_t;

/* what every primal rank carries along */
typedef enum {
  HAND_KICKER_NONE = 0,
  HAND_KICKER_SOLO,
  HAND_KICKER_PAIR,
  HAND_KICKER_DUAL_SOLO,
  HAND_KICKER_DUAL_PAIR
} hand_kicker_t;

typedef struct hand_type_s {
  hand_primal_t primal;
  hand_kicker_t kicker;
  bool chain; /* the primal part spans consecutive ranks */
} hand_type_t;

typedef enum {
  HAND_CMP_ILLEGAL = -3,
  HAND_CMP_LESS = -1,
  HAND_CMP_EQUAL = 0,
  HAND_CMP_GREATER = 1
} HandCompareResult;

/*
 * hand is a valid card set that can play.
 * cards format must be like 12345/112233/111222/1112223344/11122234 etc
 */
typedef struct hand_s {
  hand_type_t type;
  card_array_t cards;
} hand_t;

/*
 * a hand type from its three parts
 */
hand_type_t Hand_Type(hand_primal_t primal, hand_kicker_t kicker, bool chain);

/*
 * are two hand types the same
 */
bool Hand_TypeEquals(hand_type_t a, hand_type_t b);

/*
 * is the hand of exactly this type
 */
bool Hand_IsType(
    const hand_t *hand, hand_primal_t primal, hand_kicker_t kicker, bool chain);

/*
 * has the hand no type, that is, it is not a hand
 */
bool Hand_IsNone(const hand_t *hand);

/*
 * clear a hand
 */
void Hand_Clear(hand_t *hand);

/*
 * copy hands
 */
void Hand_Copy(hand_t *dst, const hand_t *src);

/*
 * parse a card array to hand, the card array is left as it is
 * returns false and leaves hand empty when the cards are not a hand
 */
bool Hand_Parse(hand_t *hand, const card_array_t *array);

/*
 * compare two hands, this is the only place that knows which hand is greater
 */
HandCompareResult Hand_Compare(const hand_t *a, const hand_t *b);

/*
 * the rank a hand is compared by: its highest primal rank
 */
int Hand_Rank(const hand_t *hand);

/*
 * the two parts of a hand: the cards that give it its rank (the trios of a
 * trio chain with kickers) and the kickers they carry, both from high to low
 */
void Hand_Split(
    const hand_t *hand, card_array_t *primal, card_array_t *kickers);

/*
 * four of a kind, beats everything but a higher bomb and the nuke
 */
bool Hand_IsBomb(const hand_t *hand);

/*
 * both jokers, beats everything
 */
bool Hand_IsNuke(const hand_t *hand);

/*
 * hand print
 */
void Hand_Print(const hand_t *hand);

#ifdef __cplusplus
}
#endif

#endif /* LANDLORD_HAND_H_ */
