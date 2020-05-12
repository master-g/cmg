#include "btree.h"

/*
 * ************************************************************
 * btree_node_t
 * ************************************************************
 */

btree_node_t *BTreeNode_Create() {
  btree_node_t *node = (btree_node_t *)malloc(sizeof(btree_node_t));
  memset(node, 0, sizeof(btree_node_t));
  if (node == NULL)
    return NULL;

  return node;
}

void BTreeNode_Destroy(btree_node_t *node) {
  Payload_Destroy(node->payload);

  free(node);
}

void BTreeNode_Print(btree_node_t *node) {
  DBGLog("%p | ", (void *)node);
  if (node == NULL) {
    printf("null \n");
    return;
  }

  Payload_Print(node->payload);
}

/*
 * ************************************************************
 * btree_t
 * ************************************************************
 */

btree_t *BTree_Create(void) {
  btree_t *ret = (btree_t *)malloc(sizeof(btree_t));
  memset(ret, 0, sizeof(btree_t));

  ret->root = NULL;

  return ret;
}

void BTree_Destroy(btree_t *t) {
  list_t *l = NULL;
  list_node_t *dnode = NULL;

  int dirFlag = 1;

  btree_node_t *current = t->root;

  if (t->root == NULL)
    return;

  if (t->root) {
    l = List_Create();
    List_PushBack_UserDefine(l, sizeof(btree_node_t **), &(t->root));

    current = t->root->left;
    while ((current != NULL) && ((current != t->root) || dirFlag)) {
      if (dirFlag) {
        List_PushBack_UserDefine(l, sizeof(btree_node_t **), &current);

        if (current->left != NULL)
          current = current->left;
        else
          dirFlag = 0;
      } else {
        if (current->right != NULL) {
          current = current->right;
          dirFlag = 1;
        } else
          current = current->parent;
      }
    }

    for (dnode = l->first; dnode != NULL; dnode = dnode->next) {
      BTreeNode_Print(*(btree_node_t **)dnode->payload->data);
      BTreeNode_Destroy(*(btree_node_t **)dnode->payload->data);
    }

    List_Destroy(l);

    free(t);
  }
}

int BTree_IsEmpty(btree_t *t) { return (int)(t->count == 0); }

btree_node_t *BTree_Find(btree_t *t, void *payload) { return NULL; }

btree_node_t *BTree_FindMin(btree_t *t) { return NULL; }

btree_node_t *BTree_FindMax(btree_t *t) { return NULL; }