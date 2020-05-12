/*
 *  Mesh.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-16.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "Mesh.h"
#include "Common.h"

void Mesh_InitWithCapacity(mesh_t *mesh, const char *name, int vertexCapacity,
                           int faceCapacity) {
  memset(mesh, 0, sizeof(mesh_t));
  Mesh_SetVertexCapacity(mesh, vertexCapacity);
  Mesh_SetFaceCapacity(mesh, faceCapacity);

  Vec3f_Clear(mesh->rotation);
  Vec3f_Clear(mesh->position);

  Mesh_SetName(mesh, name);
}

void Mesh_Copy(mesh_t *dst, const mesh_t *src) {
  Mesh_SetVertexCapacity(dst, src->vertexCapacity);
  Mesh_SetFaceCapacity(dst, src->faceCapacity);
  dst->vertexCount = src->vertexCount;
  dst->faceCount = src->faceCount;
  memcpy(dst->vertices, src->vertices, sizeof(vertex_t) * src->vertexCount);
  memcpy(dst->faces, src->faces, sizeof(face_t) * src->faceCount);
  Mesh_SetName(dst, src->name);
}

void Mesh_SetPosition(mesh_t *mesh, vec3f_t position) {
  Vec3f_Copy(mesh->position, position);
}

void Mesh_SetRotation(mesh_t *mesh, vec3f_t rotation) {
  Vec3f_Copy(mesh->rotation, rotation);
}

void Mesh_SetName(mesh_t *mesh, const char *name) {
  int namelen = (int)strlen(name);
  mesh->name = (char *)malloc(namelen + 1);
  mesh->name[namelen] = '\0';
  memcpy(mesh->name, name, namelen);
}

void Mesh_SetVertexCapacity(mesh_t *mesh, int capacity) {
  free(mesh->vertices);
  mesh->vertices = (vertex_t *)malloc(sizeof(vertex_t) * capacity);
  mesh->vertexCount = 0;
  mesh->vertexCapacity = capacity;
}

void Mesh_SetFaceCapacity(mesh_t *mesh, int capacity) {
  free(mesh->faces);
  mesh->faces = (face_t *)malloc(sizeof(face_t) * capacity);
  mesh->faceCount = 0;
  mesh->faceCapacity = capacity;
}

void Mesh_FreeBuffer(mesh_t *mesh) {
  free(mesh->vertices);
  free(mesh->faces);
  free(mesh->name);

  mesh->vertices = NULL;
  mesh->faces = NULL;
  mesh->vertexCapacity = 0;
  mesh->vertexCount = 0;
  mesh->faceCapacity = 0;
  mesh->faceCount = 0;
}

void Mesh_PushVertex(mesh_t *mesh, const vertex_t *v) {
  if (mesh->vertexCount < mesh->vertexCapacity)
    memcpy(&mesh->vertices[mesh->vertexCount++], v, sizeof(vertex_t));
}

void Mesh_PushFaceRaw(mesh_t *mesh, int a, int b, int c) {
  if (mesh->faceCount < mesh->faceCapacity) {
    mesh->faces[mesh->faceCount].a = a;
    mesh->faces[mesh->faceCount].b = b;
    mesh->faces[mesh->faceCount].c = c;
    mesh->faceCount++;
  }
}

void Mesh_ComputeFaceNormals(mesh_t *mesh) {
  int i = 0;
  for (i = 0; i < mesh->faceCount; i++) {
    vec3f_t normal;
    face_t *curFace;
    vertex_t *vertexA, *vertexB, *vertexC;

    curFace = &mesh->faces[i];
    vertexA = &mesh->vertices[curFace->a];
    vertexB = &mesh->vertices[curFace->b];
    vertexC = &mesh->vertices[curFace->c];

    Vec3f_Clear(normal);
    Vec3f_Add(normal, vertexA->normal, vertexB->normal);
    Vec3f_Add(normal, normal, vertexC->normal);
    Vec3f_Scale(normal, normal, 1 / 3.0f);
    Vec3f_Normalize(normal);

    Vec3f_Copy(curFace->normal, normal);
  }
}
