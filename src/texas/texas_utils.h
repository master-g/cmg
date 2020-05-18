/**
 * Created by MasterG on 2020/5/18.
 */

#ifndef CMG_TEXAS_UTILS_H
#define CMG_TEXAS_UTILS_H

int texas_utils_init_deck(unsigned int *deck);

void texas_utils_shuffle_deck(unsigned int *deck, unsigned int seed);

int texas_utils_next_comb(int comb[], int k, int n);

int texas_utils_str_card(unsigned int card, char *buf);

int texas_utils_str_hand(int value, char *buf);

#endif /* CMG_TEXAS_UTILS_H */
