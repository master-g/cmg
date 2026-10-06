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

#ifndef LANDLORD_LMATH_H_
#define LANDLORD_LMATH_H_

#include "common.h"

/* ************************************************************
 * MT19937 random number generator
 * ************************************************************/

#define MT19937_N 624

typedef struct mt19937_s {
  uint32_t mt[MT19937_N];
  /* state vector */
  int32_t mti; /* mti == N+1 -> mt[N] not initialized */

} mt19937_t;

void mt19937_init(mt19937_t *context, uint32_t seed);
uint32_t mt19937_uint32(mt19937_t *context);
int32_t mt19937_int32(mt19937_t *context);
/* ************************************************************
 * math
 * ************************************************************/

int lmath_next_comb(int comb[], int k, int n);

#endif /* LANDLORD_LMATH_H_ */
