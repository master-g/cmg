#ifndef BTREE_H_
#define BTREE_H_

#include "list.h"

#define btree_num_of_nodes(t) (t)->count

/*
 * ************************************************************
 * btree_node_t
 * ************************************************************
 */

typedef struct btree_node_t {
  struct btree_node_t *parent;
  struct btree_node_t *left;
  struct btree_node_t *right;

  int owned;
  const pdata *payload;

} btree_node_t;

btree_node_t *btree_node_with(const pdata *payload, int move);
void btree_node_free(const btree_node_t *node);
void btree_node_print(const btree_node_t *node);

/*
 * ************************************************************
 * btree_t
 * ************************************************************
 */

typedef struct btree_t {
  size_t size;
  int count;
  btree_node_t *root;

} btree_t;

typedef enum btree_walk_mode_t {
  INORDER = 0,
  PREORDER,
  POSTORDER,
} btree_walk_mode_t;

typedef void (*btree_walk_func)(btree_node_t *node);

btree_t *btree_alloc(void);

void btree_walk(const btree_t *t, btree_walk_mode_t mode, btree_walk_func func);

void btree_free(btree_t *t);

int btree_is_empty(const btree_t *t);

btree_node_t *
btree_find(const btree_t *t, const pdata *payload, pdata_cmp_func cmp);

btree_node_t *btree_find_min(btree_t *t, pdata_cmp_func cmp);

btree_node_t *btree_find_max(btree_t *t, pdata_cmp_func cmp);

void btree_insert(btree_t *t, btree_node_t *node);

#endif /* BTREE_H_ */
