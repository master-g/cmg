/**
 * Created by MasterG on 2020/5/18.
 */

/**
 * +--------+--------+--------+--------+
 * |xxxbbbbb|bbbbbbbb|cdhsrrrr|xxpppppp|
 * +--------+--------+--------+--------+
 *
 * p = prime number of rank (deuce=2,trey=3,four=5,five=7,...,ace=41)
 * r = rank of card (deuce=0,trey=1,four=2,five=3,...,ace=12)
 * cdhs = suit of card
 * b = bit turned on depending on rank of card
 */

#ifndef CMG_TEXAS_EVAL_H
#define CMG_TEXAS_EVAL_H

#define TEXAS_HAND_STRAIGHT_FLUSH 1
#define TEXAS_HAND_FOUR_OF_A_KIND 2
#define TEXAS_HAND_FULL_HOUSE 3
#define TEXAS_HAND_FLUSH 4
#define TEXAS_HAND_STRAIGHT 5
#define TEXAS_HAND_THREE_OF_A_KIND 6
#define TEXAS_HAND_TWO_PAIR 7
#define TEXAS_HAND_ONE_PAIR 8
#define TEXAS_HAND_HIGH_CARD 9

#define TEXAS_CARD_RANK(x) ((x >> 8u) & 0xFu)
#define TEXAS_CARD_SUIT(x) ((x) & 0xF000u)

#define TEXAS_CARD_SUIT_CLUB 0x8000u
#define TEXAS_CARD_SUIT_DIAMOND 0x4000u
#define TEXAS_CARD_SUIT_HEART 0x2000u
#define TEXAS_CARD_SUIT_SPADE 0x1000u

#define TEXAS_RANK_TEN 8
#define TEXAS_RANK_JACK 9
#define TEXAS_RANK_QUEEN 10
#define TEXAS_RANK_KING 11
#define TEXAS_RANK_ACE 12

/**
 * map the prime product of a non-flush hand to an index of
 * "texas_eval_hash_values"
 */
unsigned int texas_eval_mph_search(unsigned int product);

int texas_eval_hand_rank(unsigned short value);

int texas_eval_is_loyal_straight_flush(unsigned short value);

unsigned short texas_eval_5hand(
    unsigned int c1, unsigned int c2, unsigned int c3, unsigned int c4,
    unsigned int c5);

unsigned short texas_eval_7hand(
    unsigned int c1, unsigned int c2, unsigned int c3, unsigned int c4,
    unsigned int c5, unsigned int c6, unsigned int c7);

/**
 * map the quinary rank-count code of a non-flush 7-card hand to an index of
 * "texas_eval7_values"
 */
unsigned int texas_eval7_mph(unsigned int quinary);

/**
 * evaluate 7 cards without enumerating the 21 five-card subsets.
 *
 * a non-flush hand's rank depends only on its rank multiset, of which there
 * are just 49205; a minimal perfect hash turns that into a single lookup.
 * flushes take a separate path. same result as texas_eval_7hand, ~12x faster.
 *
 * @param cards array of 7 cards
 * @return the hand value, 1 (best) .. 7462 (worst)
 */
unsigned short texas_eval_7hand_fast(const unsigned int *cards);

#endif /* CMG_TEXAS_EVAL_H */
