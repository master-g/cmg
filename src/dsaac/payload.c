/*
 *  payload.c
 *  DSAAC
 *
 *  Created by Master.G on 13-8-18.
 *  Copyright (c) 2013 Master.G. All rights reserved.
 */

#include "payload.h"

payload_t *Payload_Create(int type, size_t size, void *data) {
  payload_t *payload = (payload_t *)malloc(sizeof(payload_t));
  memset(payload, 0, sizeof(payload_t));
  payload->type = type;
  payload->size = size;
  payload->data = malloc(size);
  memcpy(payload->data, data, size);

  return payload;
}

payload_t *Payload_CreateWithInt(int data) {
  payload_t *payload = (payload_t *)malloc(sizeof(payload_t));
  payload->type = PayloadType_Int;
  payload->size = sizeof(int);
  payload->data = malloc(sizeof(int));
  memcpy(payload->data, &data, sizeof(int));

  return payload;
}

payload_t *Payload_CreateWithChar(char data) {
  payload_t *payload = (payload_t *)malloc(sizeof(payload_t));
  payload->type = PayloadType_Char;
  payload->size = sizeof(char);
  payload->data = malloc(sizeof(char));
  memcpy(payload->data, &data, sizeof(char));

  return payload;
}

payload_t *Payload_CreateWithString(const char *data) {
  payload_t *payload = (payload_t *)malloc(sizeof(payload_t));
  payload->type = PayloadType_String;
  payload->size = strlen(data) + 1;
  payload->data = malloc(payload->size);
  memset(payload->data, 0, sizeof(payload->size));
  memcpy(payload->data, data, payload->size);

  return payload;
}

void Payload_Destroy(payload_t *payload) {
  free(payload->data);
  free(payload);
}

void Payload_Print(payload_t *payload) {
  if (payload == NULL) {
    printf("null\n");
    return;
  }

  switch (payload->type) {
  case PayloadType_Char:
    printf("char \t | %c \n", *(char *)(payload->data));
    break;
  case PayloadType_Int:
    printf("int \t | %d \n", *(int *)(payload->data));
    break;
  case PayloadType_String:
    printf("str \t | %s \n", (char *)payload->data);
    break;
  default:
    printf("Unknown payload type\n");
    break;
  }
}