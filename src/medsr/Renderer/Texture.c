/*
 *  Texture.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-24.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "Texture.h"
#include "Color.h"
#include "Math/KitsuneMath.h"

texture_t *Texture_Create(const char *rgba, int w, int h) {
  texture_t *tex = (texture_t *)calloc(1, sizeof(texture_t));
  Texture_Init(tex, rgba, w, h);

  return tex;
}

void Texture_Init(texture_t *texture, const char *rgba, int w, int h) {
  free(texture->buffer);
  texture->buffer = (char *)malloc(sizeof(char) * w * h * 4);
  memcpy(texture->buffer, rgba, w * h * 4);
  texture->width = w;
  texture->height = h;
}

void Texture_FreeBuffer(texture_t *texture) {
  free(texture->buffer);
  texture->buffer = NULL;
  texture->width = 0;
  texture->height = 0;
}

void Texture_Destroy(texture_t *texture) {
  Texture_FreeBuffer(texture);
  free(texture);
}

void Texture_Map(texture_t *texture, float tu, float tv, vec4f_t color) {
  uint8_t r, g, b, a;
  int u, v, pos;
  if (texture->buffer == NULL) {
    Color4f_Format(color, 255, 255, 255, 255);
    return;
  }

  /* using a % operator to cycle/repeat the texture if needed */
  u = i_abs((int)(tu * texture->width) % texture->width);
  v = i_abs((int)(tv * texture->height) % texture->height);

  pos = (u + v * texture->width) * 4;
  r = texture->buffer[pos + 0];
  g = texture->buffer[pos + 1];
  b = texture->buffer[pos + 2];
  a = texture->buffer[pos + 3];

  Color4f_Format(color, r, g, b, a);
}
