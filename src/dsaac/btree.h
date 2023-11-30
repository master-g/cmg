/*
 *  btree.h
 *  DSAAC
 *
 *  Created by Master.G on 23-11-30.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */
#ifndef BTREE_H_
#define BTREE_H_

#include "list.h"

typedef struct btree_t btree_t;

typedef enum btree_walk_mode {
  INORDER = 0,
  PREORDER,
  POSTORDER,
} btree_walk_mode;

typedef void (*btree_walk_func)(const pdata *data);

btree_t *btree_alloc(pdata_cmp_func cmp);

void btree_walk(const btree_t *t, btree_walk_mode mode, btree_walk_func func);

void btree_free(btree_t *t);

int btree_is_empty(const btree_t *t);

int btree_contains(const btree_t *t, const pdata *data);

const pdata *btree_find_min(const btree_t *t);

const pdata *btree_find_max(const btree_t *t);

int btree_insert(const btree_t *t, const pdata *data);

int btree_remove(const btree_t *t, const pdata *data);

#endif /* BTREE_H_ */
