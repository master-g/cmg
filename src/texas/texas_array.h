/**
 * Created by MasterG on 2020/5/18.
 */

#ifndef CMG_TEXAS_ARRAY_H
#define CMG_TEXAS_ARRAY_H

/**
 * this is a table lookup for all "flush" hands (e.g.  both
 * flushes and straight-flushes).  entries containing a zero
 * mean that combination is not possible with a five-card
 * flush hand.
 */
extern unsigned short texas_eval_flushes[];

/**
 * this is a table lookup for all non-flush hands consisting
 * of five unique ranks (i.e.  either Straights or High Card
 * hands).  it's similar to the above "flushes" array.
 */
extern unsigned short texas_eval_unique5[];

/**
 * this is a table for minimal-perfect-hash lookup
 * the prime product of hands will transform into an index
 * of "texas_eval_hash_values" via "texas_eval_mph_search"
 */
extern unsigned short texas_eval_hash_tab[];

/**
 * this is a table lookup for all other hands, Four-Of-A-Kind
 * FullHouse, Three-Of-A-Kind, Two-Pair and One-Pair
 * the index of this table is calculated by
 * "texas_eval_mph_search"
 */
extern unsigned short texas_eval_hash_values[];

/**
 * deuce = 2
 * trey = 3
 * four = 5
 * five = 7
 * ...
 * king = 37
 * ace = 41
 */
extern unsigned int texas_eval_primes[];

#endif /* CMG_TEXAS_ARRAY_H */
