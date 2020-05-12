/*
 *  Matrix.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-15.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "Matrix.h"
#include "Vector.h"

mat_t MatrixIdentity = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

/*
 * 0   4   8  12
 * 1   5   9  13
 * 2   6  10  14
 * 3   7  11  15
 */

void Matrix_Clear(mat_t m) {
  /*
  m[0] = 0.0f; m[4] = 0.0f; m[8] = 0.0f;  m[12] = 0.0f;
  m[1] = 0.0f; m[5] = 0.0f; m[9] = 0.0f;  m[13] = 0.0f;
  m[2] = 0.0f; m[6] = 0.0f; m[10] = 0.0f; m[14] = 0.0f;
  m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 0.0f;
   */
  memset(m, 0, sizeof(mat_t));
}

void Matrix_Copy(mat_t dst, const mat_t src) {
  memcpy(dst, src, sizeof(mat_t));
  /*
   dst[0] = src[0]; dst[4] = src[4]; dst[8] = src[8]; dst[12] = src[12];
   dst[1] = src[1]; dst[5] = src[5]; dst[9] = src[9]; dst[13] = src[13];
   dst[2] = src[2]; dst[6] = src[6]; dst[10] = src[10]; dst[14] = src[14];
   dst[3] = src[3]; dst[7] = src[7]; dst[11] = src[11]; dst[15] = src[15];
   */
}

int Matrix_Equal(const mat_t a, const mat_t b) {
  return ((a[0] == b[0]) && (a[4] == b[4]) && (a[8] == b[8]) &&
          (a[12] == b[12]) && (a[1] == b[1]) && (a[5] == b[5]) &&
          (a[9] == b[9]) && (a[13] == b[13]) && (a[2] == b[2]) &&
          (a[6] == b[6]) && (a[10] == b[10]) && (a[14] == b[14]) &&
          (a[3] == b[3]) && (a[7] == b[7]) && (a[11] == b[11]) &&
          (a[15] == b[15]));
}

void Matrix_Identity(mat_t m) {
  Matrix_Clear(m);
  m[0] = 1;
  m[5] = 1;
  m[10] = 1;
  m[15] = 1;
}

int Matrix_IsIdentity(mat_t m) {
  if (m[0] != 1.0 || m[5] != 1.0 || m[10] != 1.0 || m[15] != 1.0)
    return 0;

  if (m[12] != 0.0 || m[13] != 0.0 || m[14] != 0.0 || m[4] != 0.0 ||
      m[6] != 0.0 || m[7] != 0.0 || m[8] != 0.0 || m[9] != 0.0 ||
      m[11] != 0.0 || m[12] != 0.0 || m[13] != 0.0 || m[14] != 0.0)
    return 0;

  return 1;
}

float Matrix_Determinant(mat_t m) {
  float tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
  tmp1 = m[10] * m[15] - m[11] * m[14];
  tmp2 = m[9] * m[15] - m[11] * m[13];
  tmp3 = m[9] * m[14] - m[10] * m[13];
  tmp4 = m[8] * m[15] - m[11] * m[12];
  tmp5 = m[8] * m[14] - m[10] * m[12];
  tmp6 = m[8] * m[13] - m[9] * m[12];

  return ((((m[0] * (((m[5] * tmp1) - (m[6] * tmp2)) + (m[7] * tmp3))) -
            (m[1] * (((m[4] * tmp1) - (m[6] * tmp4)) + (m[7] * tmp5)))) +
           (m[2] * (((m[4] * tmp2) - (m[5] * tmp4)) + (m[7] * tmp6)))) -
          (m[3] * (((m[4] * tmp3) - (m[5] * tmp5)) + (m[6] * tmp6))));
}

void Matrix_Inverse(mat_t r, mat_t m) {
  float l1 = m[0];
  float l2 = m[1];
  float l3 = m[2];
  float l4 = m[3];
  float l5 = m[4];
  float l6 = m[5];
  float l7 = m[6];
  float l8 = m[7];
  float l9 = m[8];
  float l10 = m[9];
  float l11 = m[10];
  float l12 = m[11];
  float l13 = m[12];
  float l14 = m[13];
  float l15 = m[14];
  float l16 = m[15];
  float l17 = (l11 * l16) - (l12 * l15);
  float l18 = (l10 * l16) - (l12 * l14);
  float l19 = (l10 * l15) - (l11 * l14);
  float l20 = (l9 * l16) - (l12 * l13);
  float l21 = (l9 * l15) - (l11 * l13);
  float l22 = (l9 * l14) - (l10 * l13);
  float l23 = ((l6 * l17) - (l7 * l18)) + (l8 * l19);
  float l24 = -(((l5 * l17) - (l7 * l20)) + (l8 * l21));
  float l25 = ((l5 * l18) - (l6 * l20)) + (l8 * l22);
  float l26 = -(((l5 * l19) - (l6 * l21)) + (l7 * l22));
  float l27 = 1.0 / ((((l1 * l23) + (l2 * l24)) + (l3 * l25)) + (l4 * l26));
  float l28 = (l7 * l16) - (l8 * l15);
  float l29 = (l6 * l16) - (l8 * l14);
  float l30 = (l6 * l15) - (l7 * l14);
  float l31 = (l5 * l16) - (l8 * l13);
  float l32 = (l5 * l15) - (l7 * l13);
  float l33 = (l5 * l14) - (l6 * l13);
  float l34 = (l7 * l12) - (l8 * l11);
  float l35 = (l6 * l12) - (l8 * l10);
  float l36 = (l6 * l11) - (l7 * l10);
  float l37 = (l5 * l12) - (l8 * l9);
  float l38 = (l5 * l11) - (l7 * l9);
  float l39 = (l5 * l10) - (l6 * l9);
  r[0] = l23 * l27;
  r[4] = l24 * l27;
  r[8] = l25 * l27;
  r[12] = l26 * l27;
  r[1] = -(((l2 * l17) - (l3 * l18)) + (l4 * l19)) * l27;
  r[5] = (((l1 * l17) - (l3 * l20)) + (l4 * l21)) * l27;
  r[9] = -(((l1 * l18) - (l2 * l20)) + (l4 * l22)) * l27;
  r[13] = (((l1 * l19) - (l2 * l21)) + (l3 * l22)) * l27;
  r[2] = (((l2 * l28) - (l3 * l29)) + (l4 * l30)) * l27;
  r[6] = -(((l1 * l28) - (l3 * l31)) + (l4 * l32)) * l27;
  r[10] = (((l1 * l29) - (l2 * l31)) + (l4 * l33)) * l27;
  r[14] = -(((l1 * l30) - (l2 * l32)) + (l3 * l33)) * l27;
  r[3] = -(((l2 * l34) - (l3 * l35)) + (l4 * l36)) * l27;
  r[7] = (((l1 * l34) - (l3 * l37)) + (l4 * l38)) * l27;
  r[11] = -(((l1 * l35) - (l2 * l37)) + (l4 * l39)) * l27;
  r[15] = (((l1 * l36) - (l2 * l38)) + (l3 * l39)) * l27;
}

void Matrix_Multiply(mat_t r, const mat_t a, const mat_t b) {
  r[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
  r[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
  r[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
  r[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];

  r[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
  r[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
  r[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
  r[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];

  r[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
  r[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
  r[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
  r[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];

  r[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
  r[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
  r[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
  r[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
}

void Matrix_TripleMultiply(mat_t r, const mat_t a, const mat_t b,
                           const mat_t c) {
  mat_t ab;
  Matrix_Multiply(ab, a, b);
  Matrix_Multiply(r, ab, c);
}

void Matrix_PreMultiplyWith(mat_t a, const mat_t b) {
  mat_t t;
  Matrix_Copy(t, a);
  Matrix_Multiply(a, b, t);
}

void Matrix_PostMultiplyWith(mat_t a, const mat_t b) {
  mat_t t;
  Matrix_Copy(t, a);
  Matrix_Multiply(a, t, b);
}

void Matrix_RotationX(mat_t m, float angle) {
  float s, c;
  Matrix_Clear(m);
  s = (float)sin(angle);
  c = (float)cos(angle);
  m[0] = 1.0f;
  m[15] = 1.0f;
  m[5] = c;
  m[10] = c;
  m[9] = -s;
  m[6] = s;
}

void Matrix_RotationY(mat_t m, float angle) {
  float s, c;
  Matrix_Clear(m);
  s = (float)sin(angle);
  c = (float)cos(angle);
  m[5] = 1.0;
  m[15] = 1.0;
  m[0] = c;
  m[2] = -s;
  m[8] = s;
  m[10] = c;
}

void Matrix_RotationZ(mat_t m, float angle) {
  float s, c;
  Matrix_Clear(m);
  s = (float)sin(angle);
  c = (float)cos(angle);
  m[10] = 1.0;
  m[15] = 1.0;
  m[0] = c;
  m[1] = s;
  m[4] = -s;
  m[5] = c;
}

void Matrix_RotationAxis(mat_t m, const vec3f_t axis, float angle) {
  vec3f_t axisCopy;
  float s, c, c1;
  s = (float)sin(-angle);
  c = (float)cos(-angle);
  c1 = 1 - c;

  Vec3f_Copy(axisCopy, axis);
  Vec3f_Normalize(axisCopy);
  m[0] = (axisCopy[0] * axisCopy[0]) * c1 + c;
  m[1] = (axisCopy[0] * axisCopy[1]) * c1 - (axisCopy[2] * s);
  m[2] = (axisCopy[0] * axisCopy[2]) * c1 + (axisCopy[1] * s);
  m[3] = 0.0;
  m[4] = (axisCopy[1] * axisCopy[0]) * c1 + (axisCopy[2] * s);
  m[5] = (axisCopy[1] * axisCopy[1]) * c1 + c;
  m[6] = (axisCopy[1] * axisCopy[2]) * c1 - (axisCopy[0] * s);
  m[7] = 0.0;
  m[8] = (axisCopy[2] * axisCopy[0]) * c1 - (axisCopy[1] * s);
  m[9] = (axisCopy[2] * axisCopy[1]) * c1 + (axisCopy[0] * s);
  m[10] = (axisCopy[2] * axisCopy[2]) * c1 + c;
  m[11] = 0.0;
  m[15] = 1.0;
}

void Matrix_RotationYawPitchRoll(mat_t m, float yaw, float pitch, float roll) {
  mat_t rotY, rotX, rotZ;
  Matrix_RotationY(rotY, yaw);
  Matrix_RotationX(rotX, pitch);
  Matrix_RotationZ(rotZ, roll);
  Matrix_Multiply(m, rotX, rotZ);
  Matrix_PreMultiplyWith(m, rotY);
}

void Matrix_Scale(mat_t m, float x, float y, float z) {
  Matrix_Clear(m);
  m[0] = x;
  m[5] = y;
  m[10] = z;
  m[15] = 1.0;
}

void Matrix_Translation(mat_t m, float x, float y, float z) {
  Matrix_Identity(m);
  m[12] = x;
  m[13] = y;
  m[14] = z;
}

void Matrix_Transpose(mat_t m, const mat_t t) {
  m[0] = t[0];
  m[1] = t[4];
  m[2] = t[8];
  m[3] = t[12];
  m[4] = t[1];
  m[5] = t[5];
  m[6] = t[9];
  m[7] = t[13];
  m[8] = t[2];
  m[9] = t[6];
  m[10] = t[10];
  m[11] = t[14];
  m[12] = t[3];
  m[13] = t[7];
  m[14] = t[11];
  m[15] = t[15];
}

void Matrix_LookAtLH(mat_t m, const vec3f_t eye, const vec3f_t target,
                     const vec3f_t up) {
  float ex, ey, ez;
  vec3f_t xAxis, yAxis, zAxis;

  Vec3f_Sub(zAxis, target, eye); /* forward vector */
  Vec3f_Normalize(zAxis);
  Vec3f_CrossProduct(xAxis, up, zAxis); /* right vector */
  Vec3f_Normalize(xAxis);
  Vec3f_CrossProduct(yAxis, zAxis, xAxis); /* up vector */
  Vec3f_Normalize(yAxis);

  ex = -Vec3f_DotProduct(xAxis, eye);
  ey = -Vec3f_DotProduct(yAxis, eye);
  ez = -Vec3f_DotProduct(zAxis, eye);

  m[0] = xAxis[0];
  m[4] = xAxis[1];
  m[8] = xAxis[2];
  m[12] = ex;
  m[1] = yAxis[0];
  m[5] = yAxis[1];
  m[9] = yAxis[2];
  m[13] = ey;
  m[2] = zAxis[0];
  m[6] = zAxis[1];
  m[10] = zAxis[2];
  m[14] = ez;
  m[3] = 0;
  m[7] = 0;
  m[11] = 0;
  m[15] = 1;
}

void Matrix_PerspectiveLH(mat_t m, float width, float height, float zNear,
                          float zFar) {
  Matrix_Clear(m);
  m[0] = (2.0 * zNear) / width;
  m[1] = m[2] = m[3] = 0.0;
  m[5] = (2.0 * zNear) / height;
  m[4] = m[6] = m[7] = 0.0;
  m[10] = -zFar / (zNear - zFar);
  m[8] = m[9] = 0.0;
  m[11] = 1.0;
  m[12] = m[13] = m[15] = 0.0;
  m[14] = (zNear * zFar) / (zNear - zFar);
}

void Matrix_PerspectiveFovLH(mat_t m, float fov, float aspect, float zNear,
                             float zFar) {
  float t;
  t = 1.0f / ((float)tan(fov * 0.5));
  Matrix_Clear(m);
  m[0] = t / aspect;
  m[1] = m[2] = m[3] = 0.0f;
  m[5] = t;
  m[4] = m[6] = m[7] = 0.0f;
  m[8] = m[9] = 0.0f;
  m[10] = -zFar / (zNear - zFar);
  m[11] = 1.0f;
  m[12] = m[13] = m[15] = 0.0f;
  m[14] = (zNear * zFar) / (zNear - zFar);
}
