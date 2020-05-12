/*
 *  JsonModelLoader.h
 *  MEDSR
 *
 *  Created by Master.G on 14-4-22.
 *  Copyright (c) 2014 MED. All rights reserved.
 */

#ifndef MEDSR_JSONMODELLOADER_H_
#define MEDSR_JSONMODELLOADER_H_

#include "Renderer/Mesh.h"

void JsonModelLoader_Parse(const char *json, mesh_t **mesh, int *count);

void JsonModelLoader_LoadFromFile(const char *filename, mesh_t **mesh,
                                  int *count);

#endif /* MEDSR_JSONMODELLOADER_H_ */
