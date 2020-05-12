/*
 *  evaluator.h
 *  evaluator
 *
 *  Created by Master.G on 13-7-25.
 *  Copyright (c) 2013 MED. All rights reserved.
 */

#ifndef EVALUATOR_H
#define EVALUATOR_H

#define LEOPARD 1
#define STRAIGHT_FLUSH 2
#define FLUSH 3
#define STRAIGHT 4
#define PAIR 5
#define HIGH_CARD 6

#define RANK(x) (((x) >> 8) & 0xF)

#define CLUB 0x8000
#define DIAMOND 0x4000
#define HEART 0x2000
#define SPADE 0x1000

#define Deuce 0
#define Trey 1
#define Four 2
#define Five 3
#define Six 4
#define Seven 5
#define Eight 6
#define Nine 7
#define Ten 8
#define Jack 9
#define Queen 10
#define King 11
#define Ace 12

/*
 * combination 5 3 2 is higher than leopard
 */
#define LEOPARD_KILLER_ENTROPY 741

void eval_init_deck(int* deck);

int eval_hand_rank(short val);

const char* eval_hand_rank_description(short val);

unsigned int eval_3hand(int c1, int c2, int c3);

unsigned int eval_card_make(int suit, int rank);

#endif /* EVALUATOR_H */
