/*
 *  Vector.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-15.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef KITSUNE_VECTOR_H_
#define KITSUNE_VECTOR_H_

#include "MathUtil.h"

extern const vec3f_t Vec3fUnitX;
extern const vec3f_t Vec3fUnitY;
extern const vec3f_t Vec3fUnitZ;
extern const vec3f_t Vec3fZero;

/*
 * ************************************************************
 * vector 2
 * ************************************************************
 */

void Vec2f_Clear(vec2f_t v);

void Vec2f_Copy(vec2f_t dst, const vec2f_t src);

void Vec2f_Negate(vec2f_t a, const vec2f_t b);

void Vec2f_Set(vec2f_t v, float x, float y);

void Vec2f_Inverse(vec2f_t v);

float Vec2f_DotProduction(const vec2f_t a, const vec2f_t b);

void Vec2f_Add(vec2f_t r, const vec2f_t a, const vec2f_t b);

void Vec2f_Sub(vec2f_t r, const vec2f_t a, const vec2f_t b);

void Vec2f_Mul(vec2f_t r, const vec2f_t a, const vec2f_t b);

void Vec2f_Div(vec2f_t r, const vec2f_t a, const vec2f_t b);

void Vec2f_Scale(vec2f_t r, const vec2f_t v, float s);

float Vec2f_LengthSquare(const vec2f_t v);

float Vec2f_Length(const vec2f_t v);

void Vec2f_Max(vec2f_t r, const vec2f_t a, const vec2f_t b);

void Vec2f_Min(vec2f_t r, const vec2f_t a, const vec2f_t b);

int Vec2f_Equals(const vec2f_t a, const vec2f_t b);

void Vec2f_Swap(vec2f_t a, vec2f_t b);

void Vec2f_Transform(vec2f_t r, const vec2f_t v, const mat_t m);

void Vec2f_Normalize(vec2f_t v);

float Vec2f_DistanceSquare(const vec2f_t a, const vec2f_t b);

/*
 * ************************************************************
 * vector 3
 * ************************************************************
 */

void Vec3f_Clear(vec3f_t v);

void Vec3f_Copy(vec3f_t dst, const vec3f_t src);

void Vec3f_Negate(vec3f_t a, const vec3f_t b);

void Vec3f_Set(vec3f_t v, float x, float y, float z);

void Vec3f_Inverse(vec3f_t v);

float Vec3f_DotProduct(const vec3f_t a, const vec3f_t b);

void Vec3f_CrossProduct(vec3f_t r, const vec3f_t a, const vec3f_t b);

void Vec3f_Add(vec3f_t r, const vec3f_t a, const vec3f_t b);

void Vec3f_Sub(vec3f_t r, const vec3f_t a, const vec3f_t b);

void Vec3f_Mul(vec3f_t r, const vec3f_t a, const vec3f_t b);

void Vec3f_Div(vec3f_t r, const vec3f_t a, const vec3f_t b);

void Vec3f_Scale(vec3f_t r, const vec3f_t a, float s);

float Vec3f_LengthSquare(const vec3f_t v);

float Vec3f_Length(const vec3f_t v);

void Vec3f_Max(vec3f_t r, const vec3f_t a, const vec3f_t b);

void Vec3f_Min(vec3f_t r, const vec3f_t a, const vec3f_t b);

int Vec3f_Equals(const vec3f_t a, const vec3f_t b);

void Vec3f_Swap(vec3f_t a, vec3f_t b);

void Vec3f_Normalize(vec3f_t v);

float Vec3f_DistanceSquare(const vec3f_t a, const vec3f_t b);

void Vec3f_TransformCoordinates(vec3f_t r, const vec3f_t v, const mat_t m);

void Vec3f_Transform(vec3f_t r, const vec3f_t v, const mat_t m);

float Vec3f_NormalDotLight(vec3f_t v, const vec3f_t n, const vec3f_t l);

/*
 * ************************************************************
 * vector 4
 * ************************************************************
 */

void Vec4f_Clear(vec4f_t v);

void Vec4f_Copy(vec4f_t dst, const vec4f_t src);

void Vec4f_Negate(vec4f_t a, const vec4f_t b);

void Vec4f_Set(vec4f_t v, float x, float y, float z, float w);

void Vec4f_Inverse(vec4f_t v);

float Vec4f_DotProduct(const vec4f_t a, const vec4f_t b);

void Vec4f_Add(vec4f_t r, const vec4f_t a, const vec4f_t b);

void Vec4f_Sub(vec4f_t r, const vec4f_t a, const vec4f_t b);

void Vec4f_Mul(vec4f_t r, const vec4f_t a, const vec4f_t b);

void Vec4f_Div(vec4f_t r, const vec4f_t a, const vec4f_t b);

void Vec4f_Scale(vec4f_t r, const vec4f_t v, float s);

float Vec4f_LengthSquare(const vec4f_t v);

float Vec4f_Length(const vec4f_t v);

void Vec4f_Max(vec4f_t r, const vec4f_t a, const vec4f_t b);

void Vec4f_Min(vec4f_t r, const vec4f_t a, const vec4f_t b);

int Vec4f_Equals(const vec4f_t a, const vec4f_t b);

void Vec4f_Swap(vec4f_t a, vec4f_t b);

void Vec4f_Normalize(vec4f_t v);

float Vec4f_DistanceSquare(const vec4f_t a, const vec4f_t b);

void Vec4f_Transform(vec4f_t r, const vec4f_t v, const mat_t m);

#endif /* KITSUNE_VECTOR_H_ */
