/**
 * Created by MasterG on 2020/5/15.
 */

#include <stdlib.h>
#include <string.h>

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

static unsigned char sz_diamond[] = {0xE2, 0x99, 0xA6, 0};
static unsigned char sz_club[] = {0xE2, 0x99, 0xA3, 0};
static unsigned char sz_heart[] = {0xE2, 0x99, 0xA5, 0};
static unsigned char sz_spade[] = {0xE2, 0x99, 0xA0, 0};

int texas_utils_str_card(unsigned int card, char *buf) {
  unsigned int suit, rank;
  int i;
  unsigned char *pstr = NULL;
  if (buf == NULL) {
    return 4;
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

  rank = TEXAS_CARD_RANK(card);
  switch (rank) {
  case TEXAS_RANK_JACK:
    buf[3] = 'J';
    break;
  case TEXAS_RANK_QUEEN:
    buf[3] = 'Q';
    break;
  case TEXAS_RANK_KING:
    buf[3] = 'K';
    break;
  case TEXAS_RANK_ACE:
    buf[3] = 'A';
    break;
  default:
    buf[3] = (char)('2' + rank);
  }

  return 4;
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
