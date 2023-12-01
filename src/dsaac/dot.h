/*
 *  dot.h
 *  DSAAC
 *
 *  Created by Master.G on 23-12-01.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */

#ifndef DOT_H_
#define DOT_H_

#include <sys/types.h>

typedef struct dot_t dot_t;

dot_t *dot_alloc(void);
void dot_free(dot_t *dot);

int dot_push_head(dot_t *dot, const char *content);
int dot_unshift_head(dot_t *dot, const char *content);
int dot_push_body(dot_t *dot, const char *content);
int dot_unshift_body(dot_t *dot, const char *content);
int dot_push_foot(dot_t *dot, const char *content);
int dot_unshift_foot(dot_t *dot, const char *content);

ssize_t dot_dump(const dot_t *dot, char **buf, size_t *size);

#endif /* DOT_H_ */
