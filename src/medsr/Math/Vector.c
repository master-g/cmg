/*
 *  Vector.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-15.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "Vector.h"

const vec3f_t Vec3fUnitX = {1, 0, 0};
const vec3f_t Vec3fUnitY = {0, 1, 0};
const vec3f_t Vec3fUnitZ = {0, 0, 1};
const vec3f_t Vec3fZero = {0, 0, 0};

/*
 * ************************************************************
 * vector 2
 * ************************************************************
 */

void Vec2f_Clear(vec2f_t v) { memset(v, 0, sizeof(vec2f_t)); }

void Vec2f_Copy(vec2f_t dst, const vec2f_t src) {
  memcpy(dst, src, sizeof(vec2f_t));
}

void Vec2f_Negate(vec2f_t a, const vec2f_t b) {
  a[0] = -b[0];
  a[1] = -b[1];
}

void Vec2f_Set(vec2f_t v, float x, float y) {
  v[0] = x;
  v[1] = y;
}

void Vec2f_Inverse(vec2f_t v) {
  v[0] = -v[0];
  v[1] = -v[1];
}

float Vec2f_DotProduct(const vec2f_t a, const vec2f_t b) {
  return (a[0] * b[0] + a[1] * b[1]);
}

void Vec2f_Add(vec2f_t r, const vec2f_t a, const vec2f_t b) {
  r[0] = a[0] + b[0];
  r[1] = a[1] + b[1];
}

void Vec2f_Sub(vec2f_t r, const vec2f_t a, const vec2f_t b) {
  r[0] = a[0] - b[0];
  r[1] = a[1] - b[1];
}

void Vec2f_Mul(vec2f_t r, const vec2f_t a, const vec2f_t b) {
  r[0] = a[0] * b[0];
  r[1] = a[1] * b[1];
}

void Vec2f_Div(vec2f_t r, const vec2f_t a, const vec2f_t b) {
  r[0] = a[0] / b[0];
  r[1] = a[1] / b[1];
}

void Vec2f_Scale(vec2f_t r, const vec2f_t v, float s) {
  r[0] = v[0] * s;
  r[1] = v[1] * s;
}

float Vec2f_LengthSquare(const vec2f_t v) {
  return (v[0] * v[0] + v[1] * v[1]);
}

float Vec2f_Length(const vec2f_t v) {
  return (float)sqrt(Vec2f_LengthSquare(v));
}

void Vec2f_Max(vec2f_t r, const vec2f_t a, const vec2f_t b) {
  r[0] = f_max(a[0], b[0]);
  r[1] = f_max(a[1], b[1]);
}

void Vec2f_Min(vec2f_t r, const vec2f_t a, const vec2f_t b) {
  r[0] = f_min(a[0], b[0]);
  r[1] = f_min(a[1], b[1]);
}

int Vec2f_Equals(const vec2f_t a, const vec2f_t b) {
  return (int)((a[0] == b[0]) && (a[1] == b[1]));
}

void Vec2f_Swap(vec2f_t a, vec2f_t b) {
  vec2f_t tmp;
  tmp[0] = b[0];
  tmp[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  a[0] = tmp[0];
  a[1] = tmp[1];
}

void Vec2f_Transform(vec2f_t r, const vec2f_t v, const mat_t m) {
  r[0] = v[0] * m[0] + v[1] * m[4];
  r[1] = v[0] * m[1] + v[1] * m[5];
}

void Vec2f_Normalize(vec2f_t v) {
  float length = (float)sqrt(Vec2f_LengthSquare(v));
  v[0] /= length;
  v[1] /= length;
}

float Vec2f_DistanceSquare(const vec2f_t a, const vec2f_t b) {
  vec2f_t d;
  Vec2f_Sub(d, a, b);
  return Vec2f_LengthSquare(d);
}

/*
 * ************************************************************
 * vector 3
 * ************************************************************
 */

void Vec3f_Clear(vec3f_t v) { memset(v, 0, sizeof(vec3f_t)); }

void Vec3f_Copy(vec3f_t dst, const vec3f_t src) {
  memcpy(dst, src, sizeof(vec3f_t));
}

void Vec3f_Negate(vec3f_t a, const vec3f_t b) {
  a[0] = -b[0];
  a[1] = -b[1];
  a[2] = -b[2];
}

void Vec3f_Set(vec3f_t v, float x, float y, float z) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
}

void Vec3f_Inverse(vec3f_t v) {
  v[0] = -v[0];
  v[1] = -v[1];
  v[2] = -v[2];
}

float Vec3f_DotProduct(const vec3f_t a, const vec3f_t b) {
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

void Vec3f_CrossProduct(vec3f_t r, const vec3f_t a, const vec3f_t b) {
  vec3f_t tmpA, tmpB;
  Vec3f_Copy(tmpA, a);
  Vec3f_Copy(tmpB, b);
  r[0] = tmpA[1] * tmpB[2] - tmpA[2] * tmpB[1];
  r[1] = tmpA[2] * tmpB[0] - tmpA[0] * tmpB[2];
  r[2] = tmpA[0] * tmpB[1] - tmpA[1] * tmpB[0];
}

void Vec3f_Add(vec3f_t r, const vec3f_t a, const vec3f_t b) {
  r[0] = a[0] + b[0];
  r[1] = a[1] + b[1];
  r[2] = a[2] + b[2];
}

void Vec3f_Sub(vec3f_t r, const vec3f_t a, const vec3f_t b) {
  r[0] = a[0] - b[0];
  r[1] = a[1] - b[1];
  r[2] = a[2] - b[2];
}

void Vec3f_Mul(vec3f_t r, const vec3f_t a, const vec3f_t b) {
  r[0] = a[0] * b[0];
  r[1] = a[1] * b[1];
  r[2] = a[2] * b[2];
}

void Vec3f_Div(vec3f_t r, const vec3f_t a, const vec3f_t b) {
  r[0] = a[0] / b[0];
  r[1] = a[1] / b[1];
  r[2] = a[2] / b[2];
}

void Vec3f_Scale(vec3f_t r, const vec3f_t a, float s) {
  r[0] = a[0] * s;
  r[1] = a[1] * s;
  r[2] = a[2] * s;
}

float Vec3f_LengthSquare(const vec3f_t v) {
  return (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float Vec3f_Length(const vec3f_t v) {
  return (float)sqrt(Vec3f_LengthSquare(v));
}

void Vec3f_Max(vec3f_t r, const vec3f_t a, const vec3f_t b) {
  r[0] = f_max(a[0], b[0]);
  r[1] = f_max(a[1], b[1]);
  r[2] = f_max(a[2], b[2]);
}

void Vec3f_Min(vec3f_t r, const vec3f_t a, const vec3f_t b) {
  r[0] = f_min(a[0], b[0]);
  r[1] = f_min(a[1], b[1]);
  r[2] = f_min(a[2], b[2]);
}

int Vec3f_Equals(const vec3f_t a, const vec3f_t b) {
  return (int)((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]));
}

void Vec3f_Swap(vec3f_t a, vec3f_t b) {
  vec3f_t tmp;
  tmp[0] = b[0];
  tmp[1] = b[1];
  tmp[2] = b[2];
  b[0] = a[0];
  b[1] = a[1];
  b[2] = a[2];
  a[0] = tmp[0];
  a[1] = tmp[1];
  a[2] = tmp[2];
}

void Vec3f_Normalize(vec3f_t v) {
  float length = (float)sqrt(Vec3f_LengthSquare(v));
  v[0] /= length;
  v[1] /= length;
  v[2] /= length;
}

float Vec3f_DistanceSquare(const vec3f_t a, const vec3f_t b) {
  vec3f_t d;
  Vec3f_Sub(d, a, b);
  return Vec3f_LengthSquare(d);
}

void Vec3f_TransformCoordinates(vec3f_t r, const vec3f_t v, const mat_t m) {
  float x, y, z, w;
  x = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + m[12];
  y = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + m[13];
  z = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + m[14];
  w = v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + m[15];

  r[0] = x / w;
  r[1] = y / w;
  r[2] = z / w;
}

void Vec3f_Transform(vec3f_t r, const vec3f_t v, const mat_t m) {
  vec3f_t tmp;
  Vec3f_Copy(tmp, v);
  r[0] = tmp[0] * m[0] + tmp[1] * m[4] + tmp[2] * m[8];
  r[1] = tmp[0] * m[1] + tmp[1] * m[5] + tmp[2] * m[9];
  r[2] = tmp[0] * m[2] + tmp[1] * m[6] + tmp[2] * m[10];
}

float Vec3f_NormalDotLight(vec3f_t v, const vec3f_t n, const vec3f_t l) {
  vec3f_t lightDirection, normal;
  Vec3f_Copy(normal, n);
  Vec3f_Sub(lightDirection, l, v);
  Vec3f_Normalize(normal);
  Vec3f_Normalize(lightDirection);

  return f_max(0, Vec3f_DotProduct(normal, lightDirection));
}

/*
 * ************************************************************
 * vector 4
 * ************************************************************
 */

void Vec4f_Clear(vec4f_t v) { memset(v, 0, sizeof(vec4f_t)); }

void Vec4f_Copy(vec4f_t dst, const vec4f_t src) {
  memcpy(dst, src, sizeof(vec4f_t));
}

void Vec4f_Negate(vec4f_t a, const vec4f_t b) {
  a[0] = -b[0];
  a[1] = -b[1];
  a[2] = -b[2];
  a[3] = -b[3];
}

void Vec4f_Set(vec4f_t v, float x, float y, float z, float w) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
  v[3] = w;
}

void Vec4f_Inverse(vec4f_t v) {
  v[0] = -v[0];
  v[1] = -v[1];
  v[2] = -v[2];
  v[3] = -v[3];
}

float Vec4f_DotProduct(const vec4f_t a, const vec4f_t b) {
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]);
}

void Vec4f_Add(vec4f_t r, const vec4f_t a, const vec4f_t b) {
  r[0] = a[0] + b[0];
  r[1] = a[1] + b[1];
  r[2] = a[2] + b[2];
  r[3] = a[3] + b[3];
}

void Vec4f_Sub(vec4f_t r, const vec4f_t a, const vec4f_t b) {
  r[0] = a[0] - b[0];
  r[1] = a[1] - b[1];
  r[2] = a[2] - b[2];
  r[3] = a[3] - b[3];
}

void Vec4f_Mul(vec4f_t r, const vec4f_t a, const vec4f_t b) {
  r[0] = a[0] * b[0];
  r[1] = a[1] * b[1];
  r[2] = a[2] * b[2];
  r[3] = a[3] * b[3];
}

void Vec4f_Div(vec4f_t r, const vec4f_t a, const vec4f_t b) {
  r[0] = a[0] / b[0];
  r[1] = a[1] / b[1];
  r[2] = a[2] / b[2];
  r[3] = a[3] / b[3];
}

void Vec4f_Scale(vec4f_t r, const vec4f_t v, float s) {
  r[0] = v[0] * s;
  r[1] = v[1] * s;
  r[2] = v[2] * s;
  r[3] = v[3] * s;
}

float Vec4f_LengthSquare(const vec4f_t v) {
  return (v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

float Vec4f_Length(const vec4f_t v) {
  return (float)sqrt(Vec4f_LengthSquare(v));
}

void Vec4f_Max(vec4f_t r, const vec4f_t a, const vec4f_t b) {
  r[0] = f_max(a[0], b[0]);
  r[1] = f_max(a[1], b[1]);
  r[2] = f_max(a[2], b[2]);
  r[3] = f_max(a[3], b[3]);
}

void Vec4f_Min(vec4f_t r, const vec4f_t a, const vec4f_t b) {
  r[0] = f_min(a[0], b[0]);
  r[1] = f_min(a[1], b[1]);
  r[2] = f_min(a[2], b[2]);
  r[3] = f_min(a[3], b[3]);
}

int Vec4f_Equals(const vec4f_t a, const vec4f_t b) {
  return (int)((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]) &&
               (a[3] == b[3]));
}

void Vec4f_Swap(vec4f_t a, vec4f_t b) {
  vec4f_t tmp;
  tmp[0] = b[0];
  tmp[1] = b[1];
  tmp[2] = b[2];
  tmp[3] = b[3];
  b[0] = a[0];
  b[1] = a[1];
  b[2] = a[2];
  b[3] = a[3];
  a[0] = tmp[0];
  a[1] = tmp[1];
  a[2] = tmp[2];
  a[3] = tmp[3];
}

void Vec4f_Normalize(vec4f_t v) {
  float length = (float)sqrt(Vec4f_LengthSquare(v));
  v[0] /= length;
  v[1] /= length;
  v[2] /= length;
  v[3] /= length;
}

float Vec4f_DistanceSquare(const vec4f_t a, const vec4f_t b) {
  vec4f_t d;
  Vec4f_Sub(d, a, b);
  return Vec4f_LengthSquare(d);
}

void Vec4f_Transform(vec4f_t r, const vec4f_t v, const mat_t m) {
  vec4f_t tmp;
  Vec4f_Copy(tmp, v);
  r[0] = tmp[0] * m[0] + tmp[1] * m[4] + tmp[2] * m[8] + tmp[3] * m[12];
  r[1] = tmp[0] * m[1] + tmp[1] * m[5] + tmp[2] * m[9] + tmp[3] * m[13];
  r[2] = tmp[0] * m[2] + tmp[1] * m[6] + tmp[2] * m[10] + tmp[3] * m[14];
  r[3] = tmp[0] * m[3] + tmp[1] * m[7] + tmp[2] * m[11] + tmp[3] * m[15];
}
