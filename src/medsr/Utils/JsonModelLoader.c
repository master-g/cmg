/*
 *  JsonModelLoader.c
 *  MEDSR
 *
 *  Created by Master.G on 14-4-22.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#include "JsonModelLoader.h"
#include "Common.h"
#include "jsmn.h"

#define JSON_TOKENS 256

typedef struct mesh_list_s {
  mesh_t *mesh;
  struct mesh_list_s *next;

} mesh_list_t;

void MeshList_PushBack(mesh_list_t **ml, mesh_t *mesh) {
  mesh_list_t *node = (mesh_list_t *)malloc(sizeof(mesh_list_t));
  node->mesh = mesh;

  node->next = *ml;
  *ml = node;
}

void MeshList_Destroy(mesh_list_t **ml) {
  mesh_list_t *current = *ml;
  mesh_list_t *next;

  while (current != NULL) {
    next = current->next;
    Mesh_FreeBuffer(current->mesh);
    free(current->mesh);
    free(current);
    current = next;
  }

  *ml = NULL;
}

void MeshList_Dump(mesh_list_t *ml, mesh_t **meshes, int *count) {
  int c = 0;
  mesh_list_t *node = ml;

  while (node != NULL) {
    node = node->next;
    c++;
  }

  *meshes = (mesh_t *)calloc(c, sizeof(mesh_t));
  *count = c;

  node = ml;
  c = 0;
  while (node != NULL) {
    Mesh_ComputeFaceNormals(node->mesh);
    Mesh_Copy(&(*meshes)[c], node->mesh);
    node = node->next;
    c++;
  }
}

typedef enum {
  ParseState_NotFound = 0,
  ParseState_MeshFound,
  ParseState_NameFound,
  ParseState_SearchPosition,
  ParseState_PositionFound,
  ParseState_RotationFound,
  ParseState_uvCountFound,
  ParseState_VerticesFound,
  ParseState_LoadVertice,
  ParseState_IndicesFound,
  ParseState_LoadIndice

} ParseStatus;

jsmntok_t *Json_Tokenize(const char *js, size_t len) {
  int ret;
  unsigned int n = JSON_TOKENS;
  jsmn_parser parser;
  jsmntok_t *tokens;

  jsmn_init(&parser);

  tokens = (jsmntok_t *)malloc(sizeof(jsmntok_t) * n);

  ret = jsmn_parse(&parser, js, len, tokens, n);
  while (ret == JSMN_ERROR_NOMEM) {
    n = n * 2 + 1;
    tokens = realloc(tokens, sizeof(jsmntok_t) * n);
    ret = jsmn_parse(&parser, js, len, tokens, n);
  }

  return tokens;
}

int Json_TokenStrEqual(jsmntok_t *t, const char *js, const char *s) {
  return (strncmp(js + t->start, s, t->end - t->start) == 0 &&
          strlen(s) == (size_t)(t->end - t->start));
}

void Json_TokenToStr(jsmntok_t *t, const char *js, char **str) {
  int len = (int)(t->end - t->start);
  if (*str != NULL)
    free(*str);

  *str = (char *)malloc(len + 1);
  (*str)[len] = '\0';
  memcpy(*str, &js[t->start], len);
}

int JsonModelLoader_CountIces(const char *json) {
  int count = 0;
  const char *str = json;

  while ((*str++) != '\0') {
    if (*str == ',')
      count++;
  }

  return count;
}

void JsonModelLoader_Parse(const char *json, mesh_t **meshes, int *meshcount) {
  int i, j, idx = 0;
  char *buf = NULL;
  int uvCount = 0;
  int verticesStep = 0;
  int verticesStepCounter = 0;
  int commaCount = 0;
  int faces[3];
  vertex_t v;
  mesh_list_t *meshlist = NULL;
  mesh_t *mesh = NULL;
  ParseStatus status;
  size_t len = strlen(json);
  jsmntok_t *tokens = Json_Tokenize(json, len);

  /* parse meshes */
  status = ParseState_NotFound;

  for (i = 0, j = 1; j > 0; i++, j--) {
    jsmntok_t *t = &tokens[i];

    if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
      j += t->size;

    switch (status) {
    case ParseState_NotFound: {
      if (Json_TokenStrEqual(t, json, "meshes"))
        status = ParseState_MeshFound;

      break;
    }
    case ParseState_MeshFound: {
      if (Json_TokenStrEqual(t, json, "name"))
        status = ParseState_NameFound;

      break;
    }
    case ParseState_NameFound: {
      Json_TokenToStr(t, json, &buf);
      mesh = (mesh_t *)calloc(1, sizeof(mesh_t));
      Mesh_SetName(mesh, buf);
      MeshList_PushBack(&meshlist, mesh);
      status = ParseState_SearchPosition;

      break;
    }
    case ParseState_SearchPosition: {
      if (Json_TokenStrEqual(t, json, "position")) {
        idx = 0;
        status = ParseState_PositionFound;
      }

      break;
    }
    case ParseState_PositionFound: {
      if (Json_TokenStrEqual(t, json, "rotation")) {
        idx = 0;
        status = ParseState_RotationFound;
        break;
      }

      if (t->type != JSMN_OBJECT && t->type != JSMN_ARRAY && idx < 3) {
        Json_TokenToStr(t, json, &buf);
        mesh->position[idx++] = (float)atof(buf);
      }
    }
    case ParseState_RotationFound: {
      if (Json_TokenStrEqual(t, json, "uvCount")) {
        status = ParseState_uvCountFound;
        break;
      }

      if (t->type != JSMN_OBJECT && t->type != JSMN_ARRAY && idx < 3) {
        Json_TokenToStr(t, json, &buf);
        mesh->rotation[idx++] = (float)atof(buf);
      }

      break;
    }
    case ParseState_uvCountFound: {
      if (Json_TokenStrEqual(t, json, "vertices")) {
        idx = 0;
        status = ParseState_VerticesFound;
        break;
      }

      if (t->type != JSMN_OBJECT && t->type != JSMN_ARRAY) {
        Json_TokenToStr(t, json, &buf);
        uvCount = atoi(buf);

        switch (uvCount) {
        case 0:
          verticesStep = 6;
          break;
        case 1:
          verticesStep = 8;
          break;
        case 2:
          verticesStep = 10;
          break;
        default:
          verticesStep = 1;
          break;
        }
      }

      break;
    }
    case ParseState_VerticesFound: {
      Json_TokenToStr(t, json, &buf);
      commaCount = JsonModelLoader_CountIces(buf) + 1;
      commaCount /= verticesStep;
      Mesh_SetVertexCapacity(mesh, commaCount);
      status = ParseState_LoadVertice;
      idx = 0;
      break;
    }
    case ParseState_LoadVertice: {
      if (verticesStepCounter % verticesStep == 0)
        idx = verticesStep;

      if (idx > 0) {
        Json_TokenToStr(t, json, &buf);
        if (idx > (verticesStep - 3))
          v.coordinates[verticesStep - idx] = (float)atof(buf);
        else if (idx > (verticesStep - 6))
          v.normal[verticesStep - 3 - idx] = (float)atof(buf);
        else
          v.texCoordinates[verticesStep - 6 - idx] = (float)atof(buf);

        idx--;
        if (idx == 0)
          Mesh_PushVertex(mesh, &v);
      }
      verticesStepCounter++;

      if (Json_TokenStrEqual(t, json, "indices")) {
        idx = 0;
        status = ParseState_IndicesFound;
      }

      break;
    }
    case ParseState_IndicesFound: {
      Json_TokenToStr(t, json, &buf);
      commaCount = JsonModelLoader_CountIces(buf) + 1;
      commaCount /= 3;
      Mesh_SetFaceCapacity(mesh, commaCount);
      status = ParseState_LoadIndice;
      idx = 0;
      break;
    }
    case ParseState_LoadIndice: {
      Json_TokenToStr(t, json, &buf);
      faces[idx++] = atoi(buf);
      if (idx == 3) {
        Mesh_PushFaceRaw(mesh, faces[0], faces[1], faces[2]);
        idx = 0;
      }

      if (Json_TokenStrEqual(t, json, "subMeshes"))
        status = ParseState_NotFound;

      break;
    }

    default:
      break;
    }
  }

  MeshList_Dump(meshlist, meshes, meshcount);

  MeshList_Destroy(&meshlist);
  free(buf);
  free(tokens);
}

void JsonModelLoader_LoadFromFile(const char *filename, mesh_t **mesh,
                                  int *count) {
  FILE *fp;
  int pos, size;
  char *content = NULL;

  fp = fopen(filename, "rb");
  if (fp == NULL)
    return;

  pos = (int)ftell(fp);
  fseek(fp, 0, SEEK_END);
  size = (int)ftell(fp);
  fseek(fp, pos, SEEK_SET);

  content = (char *)malloc(size);
  if (content == NULL) {
    fclose(fp);
    return;
  }

  fread(content, 1, size, fp);
  fclose(fp);

  JsonModelLoader_Parse(content, mesh, count);
  free(content);
}
