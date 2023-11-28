/*
 *  Matrix.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-15.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef KITSUNE_MATRIX_H_
#define KITSUNE_MATRIX_H_

#include "MathUtil.h"

/*
 * column-major matrix
 * 0   4   8  12
 * 1   5   9  13
 * 2   6  10  14
 * 3   7  11  15
 */

extern mat_t MatrixIdentity;

/**
 *	@brief	clear a matrix
 *
 *	@param 	m 	the matrix need to clear
 */
void Matrix_Clear(mat_t m);

/**
 *	@brief	copy matrix to matrix
 *
 *	@param 	dst 	the destination matrix
 *	@param 	src 	the source matrix
 */
void Matrix_Copy(mat_t dst, const mat_t src);

/**
 *	@brief	check if two matrix is equal
 *
 *  @param 	a 	first matrix
 *	@param 	b 	second matrix
 */
int Matrix_Equal(const mat_t a, const mat_t b);

/**
 *	@brief	load matrix with identity
 *          1   0   0   0
 *          0   1   0   0
 *          0   0   1   0
 *          0   0   0   1
 *
 *	@param 	m 	the matrix to set identity with
 */
void Matrix_Identity(mat_t m);

/**
 *	@brief	check if matrix is identity
 *
 *	@param 	m 	the matrix to check with
 *
 *	@return	1 for identity
 */
int Matrix_IsIdentity(mat_t m);

/**
 *	@brief	calculate the determinant of the matrix
 *
 *	@param 	m 	the matrix to calculate
 *
 *	@return	|m|
 */
float Matrix_Determinant(mat_t m);

/**
 *	@brief	calculate the inverse matrix of the matrix
 *
 *	@param 	r 	result matrix, m^-1
 *	@param 	m 	the matrix to calculate with
 */
void Matrix_Inverse(mat_t r, mat_t m);

/**
 *	@brief	multiply two matrix
 *
 *	@param 	r 	result matrix
 *	@param 	a 	r = a x b
 *	@param 	b 	r = a x b
 */
void Matrix_Multiply(mat_t r, const mat_t a, const mat_t b);

/**
 *	@brief	multiply three matrices
 *
 *	@param 	r 	r = a * b * c
 *	@param 	a
 *	@param 	b
 *	@param 	c
 */
void Matrix_TripleMultiply(
    mat_t r, const mat_t a, const mat_t b, const mat_t c);

/**
 *	@brief	multiply matrix with another matrix
 *
 *	@param 	a 	a = b * a
 *	@param 	b 	a = b * a
 */
void Matrix_PreMultiplyWith(mat_t a, const mat_t b);

/**
 *	@brief	multiply matrix with another matrix
 *
 *	@param 	a 	a = a * b
 *	@param 	b 	a = a * b
 */
void Matrix_PostMultiplyWith(mat_t a, const mat_t b);

/**
 *	@brief	setup a rotation matrix around X axis
 *          1    0    0    0
 *          0    cos  -sin 0
 *          0    sin  cos  0
 *          0    0    0    1
 *
 *	@param 	m       result matrix
 *	@param 	angle 	the rotation angle
 */
void Matrix_RotationX(mat_t m, float angle);

/**
 *	@brief	setup a rotation matrix around Y axis
 *          cos  0    sin  0
 *          0    1    0    0
 *          -sin 0    cos  0
 *          0    0    0    1
 *
 *	@param 	m       result matrix
 *	@param 	angle 	the rotation angle
 */
void Matrix_RotationY(mat_t m, float angle);

/**
 *	@brief	setup a rotation matrix around Z axis
 *          cos  -sin 0    0
 *          sin  cos  0    0
 *          0    0    1    0
 *          0    0    0    1
 *
 *	@param 	m       result matrix
 *	@param 	angle 	the rotation angle
 */
void Matrix_RotationZ(mat_t m, float angle);

/**
 *	@brief	setup a rotation matrix around specific axis
 *
 *	@param 	m       result matrix
 *	@param 	axis 	the axis to rotate around
 *	@param 	angle 	the rotation angle
 */
void Matrix_RotationAxis(mat_t m, const vec3f_t axis, float angle);

/**
 *	@brief	rotation with yaw pitch and roll
 *
 *	@param 	m       result matrix
 *	@param 	yaw 	yaw
 *	@param 	pitch 	pitch
 *	@param 	roll 	roll
 */
void Matrix_RotationYawPitchRoll(mat_t m, float yaw, float pitch, float roll);

/**
 *	@brief	setup a scaling matrix
 *
 *	@param 	m 	result matrix
 *	@param 	x 	scale on x axis
 *	@param 	y 	scale on y axis
 *	@param 	z 	scale on z axis
 */
void Matrix_Scale(mat_t m, float x, float y, float z);

/**
 *	@brief	setup a translation matrix
 *
 *	@param 	m 	result matrix
 *	@param 	x 	x translate
 *	@param 	y 	y translate
 *	@param 	z 	z translate
 */
void Matrix_Translation(mat_t m, float x, float y, float z);

/**
 *	@brief	transpose a matrix
 *          m = t^T
 *
 *	@param 	m 	result matrix
 *	@param 	t 	matrix to transpose
 */
void Matrix_Transpose(mat_t m, const mat_t t);

/**
 *	@brief	setup a matrix of look at
 *
 *	@param 	m 	result matrix
 *	@param 	eye 	eye position
 *	@param 	target 	target vector
 *	@param 	up      up direction
 */
void Matrix_LookAtLH(
    mat_t m, const vec3f_t eye, const vec3f_t target, const vec3f_t up);

void Matrix_PerspectiveLH(
    mat_t m, float width, float height, float zNear, float zFar);

/**
 *	@brief	general form of the projection matrix
 *
 * uh = Cot( fov/2 ) == 1/Tan(fov/2)
 * uw / uh = 1/aspect
 *
 *   uw    0     0       0
 *    0   uh     0       0
 *    0    0    f/(f-n)  1
 *    0    0  -fn/(f-n)  0
 *
 *	@param 	m 	result matrix
 *	@param 	fov 	field of view
 *	@param 	aspect
 *	@param 	zNear
 *	@param 	zFar
 */
void Matrix_PerspectiveFovLH(
    mat_t m, float fov, float aspect, float zNear, float zFar);

#endif /* KITSUNE_MATRIX_H_ */
