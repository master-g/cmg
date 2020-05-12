/*
 *  Quarternion.h
 *  MEDSR
 *
 *  Created by Master.G on 14-5-3.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef MEDSR_QUARTERNION_H_
#define MEDSR_QUARTERNION_H_

#include "MathUtil.h"

extern const quat_t QuatIdentity;

void Quat_Clear(quat_t q);

void Quat_Copy(quat_t dst, const quat_t src);

int Quat_Equals(const quat_t a, const quat_t b);

void Quat_Identity(quat_t q);

void Quat_Set(quat_t q, float x, float y, float z, float w);

void Quat_Inverse(quat_t q);

void Quat_Negate(quat_t r, const quat_t q);

float Quat_DotProduct(const quat_t a, const quat_t b);

void Quat_Add(quat_t r, const quat_t a, const quat_t b);

void Quat_Sub(quat_t r, const quat_t a, const quat_t b);

void Quat_Mul(quat_t r, const quat_t a, const quat_t b);

void Quat_Scale(quat_t r, const quat_t q, float s);

float Quat_LengthSquare(const quat_t q);

void Quat_Normalize(quat_t q);

/**
 *	@brief	convert quaternion to matrix
 *
 * 1-2y^2-2z^2  2xy-2zw  	 2xz+2yw  	  0
 * 2xy+2zw 		1-2x^2-2z^2  2yz-2xw  	  0
 * 2xz-2yw		2yz+2xw      1-2x^2-2y^2  0
 * 0			0			 0			  1
 *
 *	@param 	q 	quarternion
 *	@param 	m 	output matrix
 */
void Quat_ToMatrix(const quat_t q, mat_t m);

#endif /* MEDSR_QUARTERNION_H_ */
