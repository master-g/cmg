/*
 *  Vertex.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-24.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "Vertex.h"
#include "Math/Vector.h"

void Vertex_Copy(vertex_t *dst, const vertex_t *src) {
  Vec3f_Copy(dst->normal, src->normal);
  Vec3f_Copy(dst->coordinates, src->coordinates);
  Vec2f_Copy(dst->texCoordinates, src->texCoordinates);
  Vec3f_Copy(dst->worldCoordinates, src->worldCoordinates);
}

void Vertex_Swap(vertex_t *a, vertex_t *b) {
  Vec3f_Swap(a->normal, b->normal);
  Vec3f_Swap(a->coordinates, b->coordinates);
  Vec2f_Swap(a->texCoordinates, b->texCoordinates);
  Vec3f_Swap(a->worldCoordinates, b->worldCoordinates);
}
