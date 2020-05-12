#ifndef BTREE_H_
#define BTREE_H_

#include "list.h"

#define BTree_Count(t) (t)->count

/*
 * ************************************************************
 * btree_node_t
 * ************************************************************
 */

typedef struct btree_node_t {
  struct btree_node_t *parent;
  struct btree_node_t *left;
  struct btree_node_t *right;

  payload_t *payload;

} btree_node_t;

btree_node_t *BTreeNode_Create();
void BTreeNode_Destroy(btree_node_t *node);

void BTreeNode_Print(btree_node_t *node);

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

btree_t *BTree_Create(void);

void BTree_Destroy(btree_t *t);

int BTree_IsEmpty(btree_t *t);

btree_node_t *BTree_Find(btree_t *t, void *payload);

btree_node_t *BTree_FindMin(btree_t *t);

btree_node_t *BTree_FindMax(btree_t *t);

void BTree_Insert(btree_t *t, btree_node_t *node);

#endif /* BTREE_H_ */
