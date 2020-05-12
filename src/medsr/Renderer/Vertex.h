/*
 *  Vertex.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-24.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef MEDSR_VERTEX_H_
#define MEDSR_VERTEX_H_

#include "Math/KitsuneMath.h"

typedef struct vertex_s {
  vec3f_t normal;
  vec3f_t coordinates;
  vec3f_t worldCoordinates;
  vec2f_t texCoordinates;

} vertex_t;

void Vertex_Copy(vertex_t *dst, const vertex_t *src);

void Vertex_Swap(vertex_t *a, vertex_t *b);

#endif /* MEDSR_VERTEX_H_ */
