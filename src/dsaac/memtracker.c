/**
 *  @file    memtracker.c
 *  @brief   memory tracker
 *
 *  @author  Master.G (MG), mg@snsteam.com
 *
 *  @internal
 *  Created:  2013/02/11
 *  Company:  MED
 *  (C) Copyright 2013 MED All rights reserved.
 *
 * This file is a part of kitsune
 *
 * The copyright to the contents herein is the property of MED
 * The contents may be used and/or copied only with the written permission of
 * MED or in accordance with the terms and conditions stipulated in
 * the agreement/contract under which the contents have been supplied.
 * =====================================================================================
 */

#define INTERNAL

#include "memtracker.h"
#include <string.h>

struct memblock {
  unsigned int magic;
  size_t size;
  const char *file;
  const char *expr;
  int line;
  int padding;
  struct memblock *next;
  struct memblock *prev;
};

#define MAGIC1 0xDEADBEEF
#define MAGIC2 0xBEEFDEAD

struct memblock *memblockList = NULL;

static void memblock_print_info(const struct memblock *mb) {
  printf(
      "%d bytes allocated with \"%s\" at %s:%d\n", (int)mb->size, mb->expr,
      mb->file, mb->line);
}

void *memtrack_malloc(
    const size_t size, const char *expr, const char *file, const int line) {
  struct memblock *mb = malloc(size + sizeof(*mb));
  if (!mb) {
    printf("Unable to malloc memory!\n");
    return NULL;
  }

  mb->magic = MAGIC1;
  mb->file = file;
  mb->line = line;
  mb->expr = expr;
  mb->size = size;
  mb->prev = NULL;
  mb->next = memblockList;
  if (memblockList) {
    memblockList->prev = mb;
  }
  memblockList = mb;
  return &mb[1];
}

void *memtrack_calloc(
    const size_t count, const size_t elem_size, const char *expr,
    const char *file, const int line) {
  struct memblock *mb = malloc(count * elem_size + sizeof(*mb));
  memset(mb, 0, count * elem_size + sizeof(*mb));

  if (!mb) {
    printf("Unable to calloc memory!\n");
    return NULL;
  }

  mb->magic = MAGIC1;
  mb->file = file;
  mb->line = line;
  mb->expr = expr;
  mb->size = count * elem_size;
  mb->prev = NULL;
  mb->next = memblockList;
  if (memblockList) {
    memblockList->prev = mb;
  }
  memblockList = mb;
  return &mb[1];
}

void *memtrack_realloc(
    void *ptr, const char *eptr, const size_t size, const char *expr,
    const char *file, const int line) {
  if (!ptr) {
    return memtrack_malloc(size, expr, file, line);
  }
  void *newPtr = NULL;
  const struct memblock *mb = &((struct memblock *)(ptr))[-1];

  if (mb->magic == MAGIC2) {
    printf("Memory has already been freed\n");
    memblock_print_info(mb);
    return NULL;
  } else if (mb->magic != MAGIC1) {
    printf(
        "Memory is not allocated in memtracker : %p (expr = \"%s\" from "
        "%s:%d\n",
        ptr, eptr, file, line);
    return NULL;
  }

  const size_t copysize = size > mb->size ? mb->size : size;

  newPtr = memtrack_malloc(size, expr, file, line);
  memcpy(newPtr, ptr, copysize);
  memtrack_free(ptr, eptr, file, line);

  return newPtr;
}

void memtrack_free(
    void *ptr, const char *expr, const char *file, const int line) {
  if (ptr) {
    struct memblock *mb = &((struct memblock *)(ptr))[-1];
    if (mb->magic != MAGIC1) {
      if (mb->magic == MAGIC2) {
        printf("Memory free more than once\n");
        memblock_print_info(mb);
      } else {
        printf(
            "Invalid free of ptr: %p (expr = \"%s\" from %s:%d\n", ptr, expr,
            file, line);
      }
      return;
    }
    mb->magic = MAGIC2;
    if (mb == memblockList) {
      memblockList = mb->next;
    }
    /* unlink */
    if (mb->next)
      mb->next->prev = mb->prev;
    if (mb->prev)
      mb->prev->next = mb->next;

    free(mb);
  }
}

void memtrack_list_allocations(void) {

  printf("*** Allocation list start ***\n");
  if (!memblockList) {
    printf(">>> EMPTY <<<\n");
  } else {
    size_t total = 0;
    for (const struct memblock *mb = memblockList; mb; mb = mb->next) {
      total += mb->size;
      memblock_print_info(mb);
    }

    printf(
        ">>>Total %ld Bytes %ld KB %ld MB<<<\n", (long)total,
        (long)(total / 1024), (long)(total / 1024 / 1024));
  }
  printf("*** Allocation list end ***\n");
}
