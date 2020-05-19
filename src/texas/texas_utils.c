/**
 * Created by MasterG on 2020/5/15.
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "texas_array.h"
#include "texas_eval.h"
#include "texas_utils.h"

int texas_utils_init_deck(unsigned int *deck) {

  if (deck != NULL) {
    unsigned int i, j, n = 0;
    unsigned int suit = 0x8000;

    for (i = 0; i < 4; i++, suit >>= 1u) {
      for (j = 0; j < 13; j++, n++) {
        deck[n] = texas_eval_primes[j] | (j << 8u) | suit | (1u << (16 + j));
      }
    }
  }

  return 52;
}

void texas_utils_shuffle_deck(unsigned int *deck, unsigned int seed) {
  int i, j;
  unsigned int tmp;
  srand(seed);
  for (i = 51; i > 0; i--) {
    j = rand() % (i + 1);
    tmp = deck[i];
    deck[i] = deck[j];
    deck[j] = tmp;
  }
}

int texas_utils_next_comb(int comb[], int k, int n) {
  int i = k - 1;
  ++comb[i];
  while ((i > 0) && (comb[i] >= n - k + 1 + i)) {
    --i;
    /* printf(">>>>>>>>> %d\n", i); */
    ++comb[i];
  }

  if (comb[0] > n - k) /* Combination (n-k, n-k+1, ..., n) reached */
    return 0;          /* No more combinations can be generated */

  /* comb now looks like (..., x, n, n, n, ..., n).
     Turn it into (..., x, x + 1, x + 2, ...) */
  for (i = i + 1; i < k; ++i)
    comb[i] = comb[i - 1] + 1;

  return 1;
}

static unsigned char sz_diamond[] = {0xE2, 0x99, 0xA6};
static unsigned char sz_club[] = {0xE2, 0x99, 0xA3};
static unsigned char sz_heart[] = {0xE2, 0x99, 0xA5};
static unsigned char sz_spade[] = {0xE2, 0x99, 0xA0};

int texas_utils_str_card(unsigned int card, char *buf) {
  unsigned int suit, rank;
  int i, len;
  unsigned char *pstr = NULL;
  if (buf == NULL) {
    return 5;
  }

  suit = TEXAS_CARD_SUIT(card);
  switch (suit) {
  case TEXAS_CARD_SUIT_DIAMOND:
    pstr = sz_diamond;
    break;
  case TEXAS_CARD_SUIT_CLUB:
    pstr = sz_club;
    break;
  case TEXAS_CARD_SUIT_HEART:
    pstr = sz_heart;
    break;
  default:
    pstr = sz_spade;
    break;
  }

  for (i = 0; i < 3; i++) {
    buf[i] = (char)pstr[i];
  }

  len = 5;

  rank = TEXAS_CARD_RANK(card);
  switch (rank) {
  case TEXAS_RANK_TEN:
    buf[3] = '1';
    buf[4] = '0';
    len = 5;
    break;
  case TEXAS_RANK_JACK:
    buf[3] = 'J';
    buf[4] = ' ';
    break;
  case TEXAS_RANK_QUEEN:
    buf[3] = 'Q';
    buf[4] = ' ';
    break;
  case TEXAS_RANK_KING:
    buf[3] = 'K';
    buf[4] = ' ';
    break;
  case TEXAS_RANK_ACE:
    buf[3] = 'A';
    buf[4] = ' ';
    break;
  default:
    buf[3] = (char)('2' + rank);
    buf[4] = ' ';
  }

  return len;
}

const char *sz_hand_straight_flush = "Straight Flush";
const char *sz_hand_four_of_a_kind = "Four of a Kind";
const char *sz_hand_full_house = "Full House";
const char *sz_hand_flush = "Flush";
const char *sz_hand_straight = "Straight";
const char *sz_hand_three_of_a_kind = "Three of a Kind";
const char *sz_two_pair = "Two Pair";
const char *sz_one_pair = "One Pair";
const char *sz_high_card = "High Card";

int texas_utils_str_hand(int value, char *buf) {
  const char *pstr = NULL;

  switch (value) {
  case TEXAS_HAND_STRAIGHT_FLUSH:
    pstr = sz_hand_straight_flush;
    break;
  case TEXAS_HAND_FOUR_OF_A_KIND:
    pstr = sz_hand_four_of_a_kind;
    break;
  case TEXAS_HAND_FULL_HOUSE:
    pstr = sz_hand_full_house;
    break;
  case TEXAS_HAND_FLUSH:
    pstr = sz_hand_flush;
    break;
  case TEXAS_HAND_STRAIGHT:
    pstr = sz_hand_straight;
    break;
  case TEXAS_HAND_THREE_OF_A_KIND:
    pstr = sz_hand_three_of_a_kind;
    break;
  case TEXAS_HAND_TWO_PAIR:
    pstr = sz_two_pair;
    break;
  case TEXAS_HAND_ONE_PAIR:
    pstr = sz_one_pair;
    break;
  default:
    pstr = sz_high_card;
    break;
  }

  if (buf == NULL) {
    return (int)strlen(pstr);
  }

  strcpy(buf, pstr);

  return (int)strlen(pstr);
}

int texas_utils_hand_list_len(texas_hand_t *list) {
  int len = 0;
  while (list != NULL) {
    list = list->next;
    len++;
  }

  return len;
}

void texas_utils_hand_list_push(texas_hand_t **list, unsigned int card1,
                                unsigned int card2, unsigned int card3,
                                unsigned int card4, unsigned int card5,
                                unsigned short value) {
  int i, sl;
  unsigned short ht;
  texas_hand_t *new_head = (texas_hand_t *)malloc(sizeof(texas_hand_t));
  new_head->cards[0] = card1;
  new_head->cards[1] = card2;
  new_head->cards[2] = card3;
  new_head->cards[3] = card4;
  new_head->cards[4] = card5;
  new_head->value = value;
  memset(new_head->str_card, 0, sizeof(new_head->str_card));
  memset(new_head->str_desc, 0, sizeof(new_head->str_desc));

  sl = 0;
  for (i = 0; i < 5; i++) {
    sl += texas_utils_str_card(new_head->cards[i], new_head->str_card + sl);
  }
  ht = texas_eval_hand_rank(value);
  texas_utils_str_hand(ht, new_head->str_desc);

  new_head->next = (*list);

  (*list) = new_head;
}

void texas_utils_hand_list_swap(texas_hand_t **a, texas_hand_t **b) {
  texas_hand_t *temp = *a;
  *a = *b;
  *b = temp;
}

void texas_utils_hand_list_merge(texas_hand_t **start1, texas_hand_t **end1,
                                 texas_hand_t **start2, texas_hand_t **end2) {
  texas_hand_t *temp = NULL;
  texas_hand_t *astart;
  texas_hand_t *bstart;
  texas_hand_t *aend;
  texas_hand_t *bendnext;

  if ((*start1)->value > (*start2)->value) {
    texas_utils_hand_list_swap(start1, start2);
    texas_utils_hand_list_swap(end1, end2);
  }

  astart = *start1;
  bstart = *start2;
  aend = *end1;
  bendnext = (*end2)->next;

  while (astart != aend && bstart != bendnext) {
    if (astart->next->value > bstart->value) {
      temp = bstart->next;
      bstart->next = astart->next;
      astart->next = bstart;
      bstart = temp;
    }
    astart = astart->next;
  }

  if (astart == aend) {
    astart->next = bstart;
  } else {
    *end2 = *end1;
  }
}

void texas_utils_hand_list_merge_sort(texas_hand_t **head) {
  texas_hand_t *start1 = NULL;
  texas_hand_t *start2 = NULL;
  texas_hand_t *end1 = NULL;
  texas_hand_t *end2 = NULL;
  texas_hand_t *prevend = NULL;
  int len, gap;

  if (*head == NULL) {
    return;
  }

  len = texas_utils_hand_list_len(*head);

  for (gap = 1; gap < len; gap = gap * 2) {
    start1 = *head;
    while (start1) {
      texas_hand_t *temp;
      int is_first_iter = 0;
      int counter = gap;

      if (start1 == *head) {
        is_first_iter = 1;
      }

      end1 = start1;
      while (--counter && end1->next) {
        end1 = end1->next;
      }

      start2 = end1->next;
      if (!start2) {
        break;
      }
      counter = gap;
      end2 = start2;
      while (--counter && end2->next) {
        end2 = end2->next;
      }

      temp = end2->next;

      texas_utils_hand_list_merge(&start1, &end1, &start2, &end2);

      if (is_first_iter) {
        *head = start1;
      } else {
        prevend->next = start1;
      }

      prevend = end2;
      start1 = temp;
    }

    prevend->next = start1;
  }
}

void texas_utils_dump_list(texas_hand_t **list) {
  int i, comb[5];
  unsigned int deck[52];

  texas_utils_init_deck(deck);
  texas_utils_shuffle_deck(deck, time(NULL));

  for (i = 0; i < 5; i++) {
    comb[i] = i;
  }

  do {
    unsigned int c1, c2, c3, c4, c5;
    unsigned short value;
    c1 = deck[comb[0]];
    c2 = deck[comb[1]];
    c3 = deck[comb[2]];
    c4 = deck[comb[3]];
    c5 = deck[comb[4]];
    value = texas_eval_5hand(c1, c2, c3, c4, c5);

    texas_utils_hand_list_push(list, c1, c2, c3, c4, c5, value);

  } while (texas_utils_next_comb(comb, 5, 52));

  texas_utils_hand_list_merge_sort(list);
}

void texas_utils_free_list(texas_hand_t *list) {
  texas_hand_t *head = list;

  while (head != NULL) {
    texas_hand_t *next = head->next;
    free(head);
    head = next;
  }
}
