/*
 *  MathUtil.h
 *  MEDSR
 *
 *  Created by Master.G on 14-5-4.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef MEDSR_MATHUTIL_H_
#define MEDSR_MATHUTIL_H_

#include "Common.h"

/*
 * ************************************************************
 * mathematic constants
 * ************************************************************
 */

#define SQRT_MAGIC_F 0x5f3759df

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#ifndef M_PI_180 /* PI/180 */
#define M_PI_180 0.017453292519943
#endif

#ifndef M_180_PI /* 180/PI */
#define M_180_PI 57.295779513082
#endif

#ifndef M_PI_2 /* pi/2           */
#define M_PI_2 1.57079632679489661923132169163975144
#endif

#ifndef M_PI_4 /* pi/4           */
#define M_PI_4 0.785398163397448309615660845819875721
#endif

#ifndef M_1_PI /* 1/pi           */
#define M_1_PI 0.318309886183790671537767526745028724
#endif

#ifndef M_2_PI /* 2/pi           */
#define M_2_PI 0.636619772367581343075535053490057448
#endif

#ifndef M_2_SQRTPI /* 2/sqrt(pi)     */
#define M_2_SQRTPI 1.12837916709551257389615890312154517
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#endif

#ifndef ABS
#define ABS(v) (((v) < 0) ? -(v) : (v))
#endif

/*
 * ************************************************************
 * mathematic structures
 * ************************************************************
 */

enum {
  VecX = 0,
  VecY,
  VecZ,
  VecW

};

enum {
  ColorR = 0,
  ColorG,
  ColorB,
  ColorA

};

enum { TexU = 0, TexV };

typedef float vec2f_t[2];
typedef float vec3f_t[3];
typedef float vec4f_t[4];

typedef vec4f_t quat_t;

typedef float mat_t[16];

/*
 * ************************************************************
 * mathematic functions
 * ************************************************************
 */
float fast_sqrt(const float x);

float f_max(float a, float b);

float f_min(float a, float b);

float f_abs(float v);

float f_clamp(float v, float _min, float _max);

float f_interpolate(float _min, float _max, float gradient);

void i_swap(int *a, int *b);

int i_max(int a, int b);

int i_min(int a, int b);

int i_abs(int v);

int i_clamp(int v, int _min, int _max);

#endif /* MEDSR_MATHUTIL_H_ */
