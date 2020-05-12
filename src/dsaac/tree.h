#ifndef TREE_H_
#define TREE_H_

#include "list.h"

#define Tree_Count(t) (t)->count

/*
 * ************************************************************
 * tree_node_t
 * ************************************************************
 */
typedef struct tree_node_t {
  struct tree_node_t *parent;
  struct tree_node_t *sibling;
  struct tree_node_t *child;

  payload_t *payload;

} tree_node_t;

tree_node_t *TreeNode_Create(void);
void TreeNode_Destroy(tree_node_t *node);

void TreeNode_Print(tree_node_t *node);

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

/* traverse function */
typedef void (*Tree_DepthTraverseCallback)(tree_node_t *node, int flag);

tree_t *Tree_Create(void);

void Tree_Destroy(tree_t *t);

int Tree_IsEmpty(tree_t *t);

void Tree_AddNode(tree_t *t, tree_node_t *parent, tree_node_t *node);

/* Test */
void Tree_Test(void);

#endif /* TREE_H_ */