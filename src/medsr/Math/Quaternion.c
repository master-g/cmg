/*
 *  Quarternion.c
 *  MEDSR
 *
 *  Created by Master.G on 14-5-3.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "Quaternion.h"

const quat_t QuatIdentity = {0, 0, 0, 1};

void Quat_Clear(quat_t q) {
  q[0] = 0.0f;
  q[1] = 0.0f;
  q[2] = 0.0f;
  q[3] = 0.0f;
}

void Quat_Copy(quat_t dst, const quat_t src) {
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
}

int Quat_Equals(const quat_t a, const quat_t b) {
  return (int)((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]) &&
               (a[3] == b[3]));
}

void Quat_Identity(quat_t q) {
  q[0] = 0.0f;
  q[1] = 0.0f;
  q[2] = 0.0f;
  q[3] = 1.0f;
}

void Quat_Set(quat_t q, float x, float y, float z, float w) {
  q[0] = x;
  q[1] = y;
  q[2] = z;
  q[3] = w;
}

void Quat_Inverse(quat_t q) {
  q[0] = -q[0];
  q[1] = -q[1];
  q[2] = -q[2];
}

void Quat_Negate(quat_t r, const quat_t q) {
  r[0] = -q[0];
  r[1] = -q[1];
  r[2] = -q[2];
  r[3] = q[3];
}

float Quat_DotProduct(const quat_t a, const quat_t b) {
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]);
}

void Quat_Add(quat_t r, const quat_t a, const quat_t b) {
  r[0] = a[0] + b[0];
  r[1] = a[1] + b[1];
  r[2] = a[2] + b[2];
  r[3] = a[3] + b[3];
}

void Quat_Sub(quat_t r, const quat_t a, const quat_t b) {
  r[0] = a[0] - b[0];
  r[1] = a[1] - b[1];
  r[2] = a[2] - b[2];
  r[3] = a[3] - b[3];
}

void Quat_Mul(quat_t r, const quat_t a, const quat_t b) {
  quat_t tmpA, tmpB;
  Quat_Copy(tmpA, a);
  Quat_Copy(tmpB, b);
  r[0] = tmpA[3] * tmpB[0] + tmpA[0] * tmpB[3] + tmpA[1] * tmpB[2] -
         tmpA[2] * tmpB[1];
  r[1] = tmpA[3] * tmpB[1] + tmpA[1] * tmpB[3] + tmpA[2] * tmpB[0] -
         tmpA[0] * tmpB[2];
  r[2] = tmpA[3] * tmpB[2] + tmpA[2] * tmpB[3] + tmpA[0] * tmpB[1] -
         tmpA[1] * tmpB[0];
  r[3] = tmpA[3] * tmpB[3] - tmpA[0] * tmpB[0] - tmpA[1] * tmpB[1] -
         tmpA[2] * tmpB[2];
}

void Quat_Scale(quat_t r, const quat_t q, float s) {
  r[0] = q[0] * s;
  r[1] = q[1] * s;
  r[2] = q[2] * s;
  r[3] = q[3] * s;
}

float Quat_LengthSquare(const quat_t q) {
  return (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

void Quat_Normalize(quat_t q) {
  float length = (float)sqrt(Quat_LengthSquare(q));
  q[0] /= length;
  q[1] /= length;
  q[2] /= length;
  q[3] /= length;
}

void Quat_ToMatrix(const quat_t q, mat_t m) {
  m[0] = 1 - 2 * (q[1] * q[1] + q[2] * q[2]);
  m[1] = 2 * (q[0] * q[1] + q[2] * q[3]);
  m[2] = 2 * (q[0] * q[2] - q[1] * q[3]);
  m[3] = 0;

  m[4] = 2 * (q[0] * q[1] - q[2] * q[3]);
  m[5] = 1 - 2 * (q[0] * q[0] + q[2] * q[2]);
  m[6] = 2 * (q[2] * q[1] + q[0] * q[3]);
  m[7] = 0;

  m[8] = 2 * (q[0] * q[2] + q[1] * q[3]);
  m[9] = 2 * (q[1] * q[2] - q[0] * q[3]);
  m[10] = 1 - 2 * (q[0] * q[0] + q[1] * q[1]);
  m[11] = 0;

  m[12] = 0;
  m[13] = 0;
  m[14] = 0;
  m[15] = 1;
}
