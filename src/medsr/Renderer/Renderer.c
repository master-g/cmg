/*
 *  Renderer.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-16.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "Renderer.h"
#include "Math/Matrix.h"

void Renderer_Init(renderer_t *renderer, int w, int h) {
  int i = 0;
  memset(renderer, 0, sizeof(renderer_t));

  renderer->width = w;
  renderer->height = h;
  renderer->bpp = GFX_BPP;
  renderer->clearColor = 0;
  renderer->backbuffer = (char *)malloc(w * h * GFX_BPP);
  renderer->depthbuffer = (float *)malloc(w * h * sizeof(float));
  renderer->clearbuffer = (char *)malloc(w * h * GFX_BPP);
  renderer->cleardepth = (float *)malloc(w * h * sizeof(float));

  for (i = 0; i < (w * h); i++)
    renderer->cleardepth[i] = FLT_MAX;
}

void Renderer_FreeBuffer(renderer_t *renderer) {
  free(renderer->backbuffer);
  free(renderer->depthbuffer);
  free(renderer->clearbuffer);
  free(renderer->cleardepth);
  renderer->backbuffer = NULL;
  renderer->depthbuffer = NULL;
}

void Renderer_ClearColor(renderer_t *renderer, uint32_t color) {
  uint8_t r, g, b, a;
  Color4i_Unpack(color, &r, &g, &b, &a);

  Renderer_ClearColor4i(renderer, r, g, b, a);
}

void Renderer_ClearColor4i(renderer_t *renderer, uint8_t r, uint8_t g,
                           uint8_t b, uint8_t a) {
  int i = 0;
  for (i = 0; i < (renderer->width * renderer->height); i++) {
    renderer->clearbuffer[i * renderer->bpp + 0] = r;
    renderer->clearbuffer[i * renderer->bpp + 1] = g;
    renderer->clearbuffer[i * renderer->bpp + 2] = b;
    renderer->clearbuffer[i * renderer->bpp + 3] = a;
  }
}

void Renderer_ClearColor4f(renderer_t *renderer, float r, float g, float b,
                           float a) {
  uint8_t ir, ig, ib, ia;
  ir = (uint8_t)(255 * r);
  ig = (uint8_t)(255 * g);
  ib = (uint8_t)(255 * b);
  ia = (uint8_t)(255 * a);

  Renderer_ClearColor4i(renderer, ir, ig, ib, ia);
}

void Renderer_Clear(renderer_t *renderer, int flag) {
  if (flag & SR_COLOR_BUFFER_BIT)
    memcpy(renderer->backbuffer, renderer->clearbuffer,
           renderer->width * renderer->height * renderer->bpp);

  if (flag & SR_DEPTH_BUFFER_BIT)
    memcpy(renderer->depthbuffer, renderer->cleardepth,
           sizeof(flag) * renderer->width * renderer->height);
}

void Renderer_PutPixel(renderer_t *renderer, int x, int y, float z,
                       uint32_t color) {
  uint8_t r, g, b, a;
  int index, depthIndex;

  depthIndex = (x + y * renderer->width);
  index = depthIndex * renderer->bpp;

  if (renderer->depthbuffer[depthIndex] < z)
    return;

  renderer->depthbuffer[depthIndex] = z;

  Color4i_Unpack(color, &r, &g, &b, &a);

  renderer->backbuffer[index + 0] = r;
  renderer->backbuffer[index + 1] = g;
  renderer->backbuffer[index + 2] = b;
  renderer->backbuffer[index + 3] = a;
}

void Renderer_DrawPixel(renderer_t *renderer, int x, int y, float z,
                        uint32_t color) {
  if (x > 0 && x <= renderer->width && y <= renderer->height && y > 0)
    Renderer_PutPixel(renderer, x, y, z, color);
}

/* Bresenham's line drawing algoritm */
void Renderer_DrawLine(renderer_t *renderer, vec3f_t p0, vec3f_t p1,
                       uint32_t color) {
  int x0, y0, x1, y1;
  int dx, dy, sx, sy, e2, err;
  x0 = (int)p0[VecX];
  y0 = (int)p0[VecY];
  x1 = (int)p1[VecX];
  y1 = (int)p1[VecY];

  dx = abs(x1 - x0);
  dy = abs(y1 - y0);
  sx = (x0 < x1) ? 1 : -1;
  sy = (y0 < y1) ? 1 : -1;
  err = dx - dy;

  while (1) {
    Renderer_DrawPixel(renderer, x0, y0, p0[VecZ], color);

    if ((x0 == x1) && (y0 == y1))
      break;

    e2 = err * 2;
    if (e2 > -dy) {
      err -= dy;
      x0 += sx;
    }

    if (e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void Renderer_Project(renderer_t *renderer, vertex_t *r, vertex_t *v,
                      mat_t trans, mat_t world) {
  vec3f_t worldvec, normalvec;
  /* transforming the coordinates into 2D space */
  Vec3f_TransformCoordinates(r->coordinates, v->coordinates, trans);
  /* transforming the coordinates & normal to the vertex in 3D world */
  Vec3f_TransformCoordinates(worldvec, v->coordinates, world);
  Vec3f_TransformCoordinates(normalvec, v->normal, world);

  r->coordinates[VecX] =
      r->coordinates[VecX] * renderer->width + renderer->width / 2.0f;
  r->coordinates[VecY] =
      -r->coordinates[VecY] * renderer->height + renderer->height / 2.0f;
  Vec3f_Copy(r->normal, normalvec);
  Vec3f_Copy(r->worldCoordinates, worldvec);
  Vec2f_Copy(r->texCoordinates, v->texCoordinates);
}

typedef struct scanline_s {
  int currentY;
  float ndotla;
  float ndotlb;
  float ndotlc;
  float ndotld;
  float ua, ub, uc, ud;
  float va, vb, vc, vd;

} scanline_t;

void Renderer_ProcessScanLine(renderer_t *renderer, scanline_t *sl,
                              vertex_t *va, vertex_t *vb, vertex_t *vc,
                              vertex_t *vd, uint32_t color,
                              texture_t *texture) {
  float gradient, gradient1, gradient2, z1, z2, z, ndotl, snl, enl, su, eu, sv,
      ev, u, v;
  int sx, ex, x;
  float *pa, *pb, *pc, *pd;
  int r, g, b, a;
  vec4f_t texColor;
  pa = va->coordinates;
  pb = vb->coordinates;
  pc = vc->coordinates;
  pd = vd->coordinates;

  gradient1 = pa[VecY] != pb[VecY]
                  ? (sl->currentY - pa[VecY]) / (pb[VecY] - pa[VecY])
                  : 1;
  gradient2 = pc[VecY] != pd[VecY]
                  ? (sl->currentY - pc[VecY]) / (pd[VecY] - pc[VecY])
                  : 1;

  /* x */
  sx = (int)f_interpolate(pa[VecX], pb[VecX], gradient1);
  ex = (int)f_interpolate(pc[VecX], pd[VecX], gradient2);

  /* z */
  z1 = f_interpolate(pa[VecZ], pb[VecZ], gradient1);
  z2 = f_interpolate(pc[VecZ], pd[VecZ], gradient2);

  /* normal */
  snl = f_interpolate(sl->ndotla, sl->ndotlb, gradient1);
  enl = f_interpolate(sl->ndotlc, sl->ndotld, gradient2);

  /* texture coordinates */
  su = f_interpolate(sl->ua, sl->ub, gradient1);
  eu = f_interpolate(sl->uc, sl->ud, gradient2);

  sv = f_interpolate(sl->va, sl->vb, gradient1);
  ev = f_interpolate(sl->vc, sl->vd, gradient2);

  Color4i_Unpack(color, &r, &g, &b, &a);

  for (x = sx; x < ex; x++) {
    gradient = (x - sx) / (float)(ex - sx);
    z = f_interpolate(z1, z2, gradient);
    ndotl = f_interpolate(snl, enl, gradient);
    u = f_interpolate(su, eu, gradient);
    v = f_interpolate(sv, ev, gradient);
    if (texture != NULL)
      Texture_Map(texture, u, v, texColor);
    else
      Color4f_Format(texColor, 255, 255, 255, 255);

    /*
     * changing the color value using the cosine of the angle
     * between the light vector and the normal vector
     */
    Renderer_DrawPixel(renderer, x, sl->currentY, z,
                       Color4i_Format((int)(r * ndotl * texColor[0]),
                                      (int)(g * ndotl * texColor[1]),
                                      (int)(b * ndotl * texColor[2]),
                                      (int)(a * ndotl * texColor[3])));
  }
}

void Renderer_DrawTriangle(renderer_t *renderer, vertex_t *v1, vertex_t *v2,
                           vertex_t *v3, uint32_t color, texture_t *texture) {
  float dP1P2, dP1P3, nl1, nl2, nl3;
  vertex_t _v1, _v2, _v3;
  float *p1, *p2, *p3;
  vec3f_t lightPos;
  scanline_t data;
  int y;

  Vertex_Copy(&_v1, v1);
  Vertex_Copy(&_v2, v2);
  Vertex_Copy(&_v3, v3);

  if (_v1.coordinates[VecY] > _v2.coordinates[VecY])
    Vertex_Swap(&_v1, &_v2);

  if (_v2.coordinates[VecY] > _v3.coordinates[VecY])
    Vertex_Swap(&_v2, &_v3);

  if (_v1.coordinates[VecY] > _v2.coordinates[VecY])
    Vertex_Swap(&_v1, &_v2);

  p1 = _v1.coordinates;
  p2 = _v2.coordinates;
  p3 = _v3.coordinates;

  /* light position */
  Vec3f_Set(lightPos, 0, 10, 10);
  /*
   * computing the cos of the angle between the light vector and the normal
   * vector it will return a value between 0 and 1 that will be used as the
   * intensity of the color
   */
  nl1 = Vec3f_NormalDotLight(_v1.worldCoordinates, _v1.normal, lightPos);
  nl2 = Vec3f_NormalDotLight(_v2.worldCoordinates, _v2.normal, lightPos);
  nl3 = Vec3f_NormalDotLight(_v3.worldCoordinates, _v3.normal, lightPos);

  /* computing lines' directions */
  if (p2[VecY] - p1[VecY] > 0.0f)
    dP1P2 = (p2[VecX] - p1[VecX]) / (p2[VecY] - p1[VecY]);
  else
    dP1P2 = 0.0f;

  if (p3[VecY] - p1[VecY] > 0.0f)
    dP1P3 = (p3[VecX] - p1[VecX]) / (p3[VecY] - p1[VecY]);
  else
    dP1P3 = 0.0f;

  if (dP1P2 > dP1P3) {
    for (y = (int)p1[VecY]; y <= (int)p3[VecY]; y++) {
      data.currentY = y;

      if (y < p2[VecY]) {
        data.ndotla = nl1;
        data.ndotlb = nl3;
        data.ndotlc = nl1;
        data.ndotld = nl2;

        data.ua = _v1.texCoordinates[VecX];
        data.ub = _v3.texCoordinates[VecX];
        data.uc = _v1.texCoordinates[VecX];
        data.ud = _v2.texCoordinates[VecX];

        data.va = _v1.texCoordinates[VecY];
        data.vb = _v3.texCoordinates[VecY];
        data.vc = _v1.texCoordinates[VecY];
        data.vd = _v2.texCoordinates[VecY];

        Renderer_ProcessScanLine(renderer, &data, &_v1, &_v3, &_v1, &_v2, color,
                                 texture);
      } else {
        data.ndotla = nl1;
        data.ndotlb = nl3;
        data.ndotlc = nl2;
        data.ndotld = nl3;

        data.ua = _v1.texCoordinates[VecX];
        data.ub = _v3.texCoordinates[VecX];
        data.uc = _v2.texCoordinates[VecX];
        data.ud = _v3.texCoordinates[VecX];

        data.va = _v1.texCoordinates[VecY];
        data.vb = _v3.texCoordinates[VecY];
        data.vc = _v2.texCoordinates[VecY];
        data.vd = _v3.texCoordinates[VecY];

        Renderer_ProcessScanLine(renderer, &data, &_v1, &_v3, &_v2, &_v3, color,
                                 texture);
      }
    }
  } else {
    for (y = (int)p1[VecY]; y <= (int)p3[VecY]; y++) {
      data.currentY = y;

      if (y < p2[VecY]) {
        data.ndotla = nl1;
        data.ndotlb = nl2;
        data.ndotlc = nl1;
        data.ndotld = nl3;

        data.ua = _v1.texCoordinates[VecX];
        data.ub = _v2.texCoordinates[VecX];
        data.uc = _v1.texCoordinates[VecX];
        data.ud = _v3.texCoordinates[VecX];

        data.va = _v1.texCoordinates[VecY];
        data.vb = _v2.texCoordinates[VecY];
        data.vc = _v1.texCoordinates[VecY];
        data.vd = _v3.texCoordinates[VecY];

        Renderer_ProcessScanLine(renderer, &data, &_v1, &_v2, &_v1, &_v3, color,
                                 texture);
      } else {
        data.ndotla = nl2;
        data.ndotlb = nl3;
        data.ndotlc = nl1;
        data.ndotld = nl3;

        data.ua = _v2.texCoordinates[VecX];
        data.ub = _v3.texCoordinates[VecX];
        data.uc = _v1.texCoordinates[VecX];
        data.ud = _v3.texCoordinates[VecX];

        data.va = _v2.texCoordinates[VecY];
        data.vb = _v3.texCoordinates[VecY];
        data.vc = _v1.texCoordinates[VecY];
        data.vd = _v3.texCoordinates[VecY];

        Renderer_ProcessScanLine(renderer, &data, &_v2, &_v3, &_v1, &_v3, color,
                                 texture);
      }
    }
  }
}

void Renderer_RenderMesh(renderer_t *renderer, mesh_t *meshes, int length) {
  int i, f;
  mat_t viewMatrix;
  mat_t projectionMatrix;
  mat_t translateMatrix;
  mat_t rypMatrix;
  mat_t worldMatrix;
  mat_t worldView;
  mat_t transformMatrix;

  Matrix_LookAtLH(viewMatrix, renderer->camera.position,
                  renderer->camera.target, Vec3fUnitY);
  Matrix_PerspectiveFovLH(projectionMatrix, 0.78f,
                          (float)renderer->width / renderer->height, 0.01f,
                          1.0f);

  for (i = 0; i < length; i++) {
    Matrix_Translation(translateMatrix, meshes[i].position[VecX],
                       meshes[i].position[VecY], meshes[i].position[VecZ]);
    Matrix_RotationYawPitchRoll(rypMatrix, meshes[i].rotation[VecY],
                                meshes[i].rotation[VecX],
                                meshes[i].rotation[VecZ]);
    Matrix_Multiply(worldMatrix, translateMatrix, rypMatrix);

    Matrix_Multiply(worldView, viewMatrix, worldMatrix);
    Matrix_Multiply(transformMatrix, projectionMatrix, worldView);

    for (f = 0; f < meshes[i].faceCount; f++) {
      vec3f_t transformedNormal;
      face_t *curFace = &meshes[i].faces[f];
      vertex_t pointA, pointB, pointC;
      vertex_t *vertexA = Mesh_GetVertex(&meshes[i], curFace->a);
      vertex_t *vertexB = Mesh_GetVertex(&meshes[i], curFace->b);
      vertex_t *vertexC = Mesh_GetVertex(&meshes[i], curFace->c);

      Vec3f_Transform(transformedNormal, curFace->normal, worldView);
      if (transformedNormal[VecZ] <= 0.0f) {
        Renderer_Project(renderer, &pointA, vertexA, transformMatrix,
                         worldMatrix);
        Renderer_Project(renderer, &pointB, vertexB, transformMatrix,
                         worldMatrix);
        Renderer_Project(renderer, &pointC, vertexC, transformMatrix,
                         worldMatrix);

        Renderer_DrawTriangle(renderer, &pointA, &pointB, &pointC, 0xFFFFFFFF,
                              meshes[i].texture);
      }
    }
  }
}
