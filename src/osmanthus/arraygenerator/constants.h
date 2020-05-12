/*
 *  constants.h
 *  arraygenerator
 *
 *  Created by Master.G on 13-7-25.
 *  Copyright (c) 2013 SNSTEAM. All rights reserved.
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

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

#endif /* CONSTANTS_H_ */
