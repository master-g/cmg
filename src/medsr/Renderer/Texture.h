/*
 *  Texture.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-24.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef MEDSR_TEXTURE_H_
#define MEDSR_TEXTURE_H_

#include "Math/KitsuneMath.h"

typedef struct texture_s {
  int width;
  int height;
  char *buffer;

} texture_t;

texture_t *Texture_Create(const char *rgba, int w, int h);

void Texture_Init(texture_t *texture, const char *rgba, int w, int h);

void Texture_FreeBuffer(texture_t *texture);

void Texture_Destroy(texture_t *texture);

void Texture_Map(texture_t *texture, float tu, float tv, vec4f_t color);

#endif /* MEDSR_TEXTURE_H_ */
