/**
 * Created by MasterG on 2020/5/18.
 */

#ifndef CMG_TEXAS_UTILS_H
#define CMG_TEXAS_UTILS_H

typedef struct texas_hand_s {
  unsigned int cards[5];
  unsigned short value;
  char str_card[26];
  char str_desc[20];
  struct texas_hand_s *next;
} texas_hand_t;

int texas_utils_init_deck(unsigned int *deck);

void texas_utils_shuffle_deck(unsigned int *deck, unsigned int seed);

int texas_utils_next_comb(int comb[], int k, int n);

int texas_utils_str_card(unsigned int card, char *buf);

int texas_utils_str_hand(int value, char *buf);

void texas_utils_dump_list(texas_hand_t **list);

void texas_utils_free_list(texas_hand_t *list);

#endif /* CMG_TEXAS_UTILS_H */
