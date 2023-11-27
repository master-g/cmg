/*
 *  payload.h
 *  DSAAC
 *
 *  Created by Master.G on 13-8-18.
 *  Copyright (c) 2013 Master.G. All rights reserved.
 */

#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include "common.h"

typedef enum {
  PayloadType_Char = 0,
  PayloadType_Int,
  PayloadType_String,
  PayloadType_UserDefine,

  PayloadType_Count

} PayloadType;

typedef struct payload_t {
  int type;
  size_t size;
  void *data;

} payload_t;

typedef int (*Payload_CompareFunc)(payload_t *payload);

extern Payload_CompareFunc PayloadComparators[PayloadType_Count];

payload_t *Payload_Create(int type, size_t size, void *data);
payload_t *Payload_CreateWithInt(int data);
payload_t *Payload_CreateWithChar(char data);
payload_t *Payload_CreateWithString(const char *data);

void Payload_Destroy(payload_t *payload);

void Payload_Print(payload_t *payload);

#endif /* PAYLOAD_H_ */
