/*
 *  utils.h
 *  arraygenerator
 *
 *  Created by Master.G on 13-7-25.
 *  Copyright (c) 2013 SNSTEAM. All rights reserved.
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int64_t GetTimeInMicroseconds();

/*
 next_comb(int comb[], int k, int n)
 Generates the next combination of n elements as k after comb

 comb => the previous combination ( use (0, 1, 2, ..., k) for first)
 k => the size of the subsets to generate
 n => the size of the original set

 Returns: 1 if a valid combination was found
 0, otherwise
 */
int next_comb(int comb[], int k, int n);

void print_rank(int card);
void print_suit(int card);
void print_card(int card);

#endif /* UTILS_H_ */
