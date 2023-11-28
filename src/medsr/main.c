/*
 *  main.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-15.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "Common.h"
#include "Math/KitsuneMath.h"
#include "Renderer/Renderer.h"
#include "Utils/Bitmap.h"
#include "Utils/Gif.h"
#include "Utils/JsonModelLoader.h"
#include "Utils/LodePng.h"

int main(int argc, const char *argv[]) {
  renderer_t renderer;
  mesh_t *meshes = NULL;
  vec3f_t vertex;
  gif_writer_t writer;
  texture_t texture;
  int texWidth, texHeight;
  unsigned char *texData = NULL;
  int i = 0;
  int frame = 0;
  int meshcount = 0;

  /* setup renderer */
  Renderer_Init(&renderer, 640, 480);
  Renderer_ClearColor(&renderer, ColorPreset4i_Azure);

  /* setup mesh */
  JsonModelLoader_LoadFromFile("suzanne.babylon", &meshes, &meshcount);
  Vec3f_Set(meshes[0].rotation, 0, 135.0f, 0);

  texture.buffer = NULL;
  lodepng_decode32_file(
      &texData, (unsigned int *)&texWidth, (unsigned int *)&texHeight,
      "suzanne.png");
  Texture_Init(&texture, (char *)texData, texWidth, texHeight);
  meshes[0].texture = &texture;

  Vec3f_Set(vertex, 0, 0, 10.0f);
  Camera_SetPosition(&renderer.camera, vertex);
  Camera_SetTarget(&renderer.camera, Vec3fZero);

  Gif_Begin(
      &writer, "render.gif", renderer.width, renderer.height,
      GIF_DEFAULT_FRAME_DURATION, renderer.bpp, 0);
  printf("%d\n", (int)time(NULL));
  for (frame = 0; frame < 120; frame++) {
    Renderer_Clear(&renderer, SR_COLOR_BUFFER_BIT | SR_DEPTH_BUFFER_BIT);
    Renderer_RenderMesh(&renderer, meshes, 1);
    meshes[0].rotation[1] += 0.1f;

    Gif_WriteFrame(
        &writer, (uint8_t *)renderer.backbuffer, renderer.width,
        renderer.height, GIF_DEFAULT_FRAME_DURATION, renderer.bpp, 0);
  }
  printf("%d\n", (int)time(NULL));

  Gif_End(&writer);

  Bitmap_Dump("dump.bmp", renderer.width, renderer.height, renderer.backbuffer);

  Renderer_FreeBuffer(&renderer);

  for (i = 0; i < meshcount; i++)
    Mesh_FreeBuffer(&meshes[i]);

  free(meshes);
  Texture_FreeBuffer(&texture);
  free(texData);

  memtrack_list_allocations();

  return 0;
}
