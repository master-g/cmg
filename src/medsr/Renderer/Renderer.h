/*
 *  Renderer.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-16.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef MEDSR_RENDERER_H_
#define MEDSR_RENDERER_H_

#include "Camera.h"
#include "Color.h"
#include "Mesh.h"

#define GFX_BPP 4

#define SR_COLOR_BUFFER_BIT 0x01
#define SR_DEPTH_BUFFER_BIT 0x02

typedef struct renderer_s {
  camera_t camera;
  mesh_t mesh;
  int width;
  int height;
  int bpp;
  uint32_t clearColor;
  char *backbuffer;
  char *clearbuffer;
  float *depthbuffer;
  float *cleardepth;

} renderer_t;

void Renderer_Init(renderer_t *renderer, int w, int h);

void Renderer_FreeBuffer(renderer_t *renderer);

void Renderer_ClearColor(renderer_t *renderer, uint32_t color);

void Renderer_ClearColor4i(renderer_t *renderer, uint8_t r, uint8_t g,
                           uint8_t b, uint8_t a);

void Renderer_ClearColor4f(renderer_t *renderer, float r, float g, float b,
                           float a);

void Renderer_Clear(renderer_t *renderer, int flag);

void Renderer_PutPixel(renderer_t *renderer, int x, int y, float z,
                       uint32_t color);

void Renderer_DrawPixel(renderer_t *renderer, int x, int y, float z,
                        uint32_t color);

void Renderer_DrawLine(renderer_t *renderer, vec3f_t p0, vec3f_t p1,
                       uint32_t color);

void Renderer_DrawTriangle(renderer_t *renderer, vertex_t *v1, vertex_t *v2,
                           vertex_t *v3, uint32_t color, texture_t *texture);

void Renderer_Project(renderer_t *renderer, vertex_t *r, vertex_t *v,
                      mat_t trans, mat_t world);

void Renderer_RenderMesh(renderer_t *renderer, mesh_t *meshes, int length);

#endif /* MEDSR_RENDERER_H_ */
