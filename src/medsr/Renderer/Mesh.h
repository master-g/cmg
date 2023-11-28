/*
 *  Mesh.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-16.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef KITSUNE_MESH_H_
#define KITSUNE_MESH_H_

#include "Math/Vector.h"
#include "Texture.h"
#include "Vertex.h"

#define VERTEX_COUNT 3

#define Mesh_PushFace(m, f) (Mesh_PushFaceRaw((m), (f)->a, (f)->b, (f)->c))

#define Mesh_GetVertex(m, i) (&(m)->vertices[(i)])

#define Face_Set(f, a, b, c) ((f)->a = (a), (f)->b = (b), (f)->c = (c))

typedef struct face_s {
  int a;
  int b;
  int c;
  vec3f_t normal;
} face_t;

typedef struct mesh_s {
  int vertexCapacity;
  int vertexCount;
  int faceCapacity;
  int faceCount;
  vec3f_t position;
  vec3f_t rotation;
  char *name;
  vertex_t *vertices;
  face_t *faces;
  texture_t *texture;

} mesh_t;

void Mesh_InitWithCapacity(
    mesh_t *mesh, const char *name, int vertexCapacity, int faceCapacity);

void Mesh_Copy(mesh_t *dst, const mesh_t *src);

void Mesh_SetPosition(mesh_t *mesh, vec3f_t position);

void Mesh_SetRotation(mesh_t *mesh, vec3f_t rotation);

void Mesh_SetVertexCapacity(mesh_t *mesh, int capacity);

void Mesh_SetFaceCapacity(mesh_t *mesh, int capacity);

void Mesh_SetName(mesh_t *mesh, const char *name);

void Mesh_FreeBuffer(mesh_t *mesh);

void Mesh_PushVertex(mesh_t *mesh, const vertex_t *v);

void Mesh_PushFaceRaw(mesh_t *mesh, int a, int b, int c);

void Mesh_ComputeFaceNormals(mesh_t *mesh);

#endif /* KITSUNE_MESH_H_ */
