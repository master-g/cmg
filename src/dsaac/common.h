/*
 *  common.h
 *  DSAAC
 *
 *  Created by Master.G on 23-11-30.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */
#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KITSUNE_DEBUG
#include "memtracker.h"

#define DBGLog(s, ...)                                                         \
  debug_printf(__FILE__, __LINE__, __func__), printf(s, ##__VA_ARGS__)
static void debug_printf(const char *file, int line, const char *func) {
  size_t slashPos = strlen(file);

  while (*(file + slashPos) != '/')
    slashPos--;

  printf("[%-25s| %-4d |%-28s] ", file + slashPos + 1, line, func);
}

#endif /* COMMON_H_ */
