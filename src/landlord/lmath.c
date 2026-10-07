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

#include "lmath.h"

/* ************************************************************
 * MT19937 random number generator
 * ************************************************************/

#define MT19937_M 397
#define MT19937_MATRIX_A 0x9908B0DF   /* constant vector A */
#define MT19937_UPPER_MASK 0x80000000 /* most significant w-r bits */
#define MT19937_LOWER_MASK 0x7FFFFFFF /* least significant w-r bits */

#define MT19937_FULL_MASK 0xFFFFFFFF

void mt19937_init(mt19937_t *context, uint32_t seed) {
  context->mt[0] = seed & MT19937_FULL_MASK;

  for (context->mti = 1; context->mti < MT19937_N; context->mti++) {
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    context->mt[context->mti] =
        (1812433253 * (context->mt[context->mti - 1] ^
                       (context->mt[context->mti - 1] >> 30)) +
         (uint32_t)context->mti);
    context->mt[context->mti] &= MT19937_FULL_MASK;
  }
}

uint32_t mt19937_uint32(mt19937_t *context) {
  uint32_t y;
  int kk;
  static uint32_t mag01[2] = {0x0, MT19937_MATRIX_A};

  /* mag01[x] = x * MT19937_MATRIX_A for x = 0, 1 */

  if (context->mti >= MT19937_N) {
    if (context->mti == MT19937_N + 1)
      mt19937_init(context, 5489);

    for (kk = 0; kk < MT19937_N - MT19937_M; kk++) {
      y = (context->mt[kk] & MT19937_UPPER_MASK) |
          (context->mt[kk + 1] & MT19937_LOWER_MASK);
      context->mt[kk] = context->mt[kk + MT19937_M] ^ (y >> 1) ^ mag01[y & 0x1];
    }

    for (; kk < MT19937_N - 1; kk++) {
      y = (context->mt[kk] & MT19937_UPPER_MASK) |
          (context->mt[kk + 1] & MT19937_LOWER_MASK);
      context->mt[kk] =
          context->mt[kk + (MT19937_M - MT19937_N)] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    y = (context->mt[MT19937_N - 1] & MT19937_UPPER_MASK) |
        (context->mt[0] & MT19937_LOWER_MASK);
    context->mt[MT19937_N - 1] =
        context->mt[MT19937_M - 1] ^ (y >> 1) ^ mag01[y & 0x1];

    context->mti = 0;
  }

  y = context->mt[context->mti++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9D2C5680;
  y ^= (y << 15) & 0xEFC60000;
  y ^= (y >> 18);

  return y;
}

int32_t mt19937_int32(mt19937_t *context) {
  return (int32_t)(mt19937_uint32(context) >> 1);
}
