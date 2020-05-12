/*
 *  MathUtil.c
 *  MEDSR
 *
 *  Created by Master.G on 14-5-4.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "MathUtil.h"

float fast_sqrt(float x) {
  const float xhalf = 0.5f * x;

  union /* get bits for floating value */
  {
    float x;
    int i;
  } u;
  u.x = x;
  u.i = SQRT_MAGIC_F - (u.i >> 1); /* gives initial guess y0 */
  return x * u.x *
         (1.5f -
          xhalf * u.x * u.x); /* Newton step, repeating increases accuracy */
}

float f_max(float a, float b) { return (a > b ? a : b); }

float f_min(float a, float b) { return (a > b ? b : a); }

float f_abs(float v) { return (v < 0 ? -v : v); }

float f_clamp(float v, float _min, float _max) {
  return f_max(_min, f_min(v, _max));
}

float f_interpolate(float _min, float _max, float gradient) {
  return (_min + (_max - _min) * f_clamp(gradient, 0.0f, 1.0f));
}

void i_swap(int *a, int *b) {
  *a = *a ^ *b;
  *b = *a ^ *b;
  *a = *a ^ *b;
}

int i_max(int a, int b) { return (a > b ? a : b); }

int i_min(int a, int b) { return (a > b ? b : a); }

int i_abs(int v) { return (v < 0 ? -v : v); }

int i_clamp(int v, int _min, int _max) { return i_max(_min, i_min(v, _max)); }
