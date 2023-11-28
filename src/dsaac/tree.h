#ifndef TREE_H_
#define TREE_H_

#include "list.h"

#define tree_num_of_nodes(t) (t)->count

/*
 * ************************************************************
 * tree_node_t
 * ************************************************************
 */
typedef struct tree_node_t {
  struct tree_node_t *parent;
  struct tree_node_t *sibling;
  struct tree_node_t *child;

  int own_pdata;
  const pdata *data;

} tree_node_t;

tree_node_t *tree_node_with(const pdata *payload, int move);
void tree_node_free(tree_node_t *node);
void tree_node_print(tree_node_t *node);

/*
 * ************************************************************
 * tree_t
 * ************************************************************
 */

typedef struct tree_t {
  size_t size;
  int count;
  tree_node_t *root;

} tree_t;

tree_t *tree_alloc(void);

void tree_free(tree_t *t);

int tree_is_empty(const tree_t *t);

void tree_insert(tree_t *t, tree_node_t *parent, tree_node_t *node);

/* Test */
void tree_test(void);

#endif /* TREE_H_ */
