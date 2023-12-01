/*
 *  btree.c
 *  DSAAC
 *
 *  Created by Master.G on 23-11-30.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */
#include "btree.h"
#include "stack.h"

/*
 * ************************************************************
 * btree_node_t
 * ************************************************************
 */
typedef struct btree_node_t {
  struct btree_node_t *parent;
  struct btree_node_t *left;
  struct btree_node_t *right;
  const pdata *payload;

} btree_node_t;

btree_node_t *btree_node_with(const pdata *payload) {
  btree_node_t *node = malloc(sizeof(btree_node_t));
  memset(node, 0, sizeof(btree_node_t));
  if (node == NULL)
    return NULL;

  node->payload = payload;

  return node;
}

void btree_node_free(const pdata *node) { free(pdata_raw(node)); }

/*
 * ************************************************************
 * btree_t
 * ************************************************************
 */

struct btree_t {
  btree_node_t *root;
  pdata_cmp_func cmp;
};

btree_t *btree_alloc(const pdata_cmp_func cmp) {
  btree_t *ret = malloc(sizeof(btree_t));
  memset(ret, 0, sizeof(btree_t));

  ret->root = NULL;
  ret->cmp = cmp;

  return ret;
}

void _btree_walk_inorder(const btree_t *t, const btree_walk_func func) {
  stack_t *s = stack_alloc();
  const pdata *stack_top = NULL;
  btree_node_t *current = t->root;

  while (current != NULL || !stack_is_empty(s)) {
    // current and its left children
    while (current != NULL) {
      stack_push(s, pdata_from_ref(current));
      current = current->left;
    }
    // pop and visit
    stack_top = stack_pop(s);
    current = (btree_node_t *)pdata_raw(stack_top);
    func(current->payload);
    pdata_free(stack_top);

    // handle right children
    current = current->right;
  }

  stack_free(s);
}

void _btree_walk_preorder(const btree_t *t, const btree_walk_func func) {
  stack_t *s = stack_alloc();
  const pdata *stack_top = NULL;

  btree_node_t *current = t->root;
  while (current != NULL || !stack_is_empty(s)) {
    if (current != NULL) {
      // visit
      func(current->payload);
      // push
      stack_push(s, pdata_from_ref(current));
      // handle left children
      current = current->left;
    } else {
      // handle right children
      stack_top = stack_pop(s);
      current = (btree_node_t *)pdata_raw(stack_top);
      pdata_free(stack_top);

      // handle right children
      current = current->right;
    }
  }

  stack_free(s);
}

void _btree_walk_postorder(const btree_t *t, const btree_walk_func func) {
  stack_t *s = stack_alloc();
  const pdata *stack_top = NULL;
  btree_node_t *current = t->root;
  const btree_node_t *last_visited = NULL;

  while (current != NULL || !stack_is_empty(s)) {
    if (current != NULL) {
      // push
      stack_push(s, pdata_from_ref(current));
      // handle left children
      current = current->left;
    } else {

      const btree_node_t *peek_node = pdata_raw(stack_peek(s));
      // check if right children has been already visited
      if (peek_node->right != NULL && last_visited != peek_node->right) {
        // handle right children
        current = peek_node->right;
      } else {
        // visit
        func(peek_node->payload);
        // pop
        stack_top = stack_pop(s);
        last_visited = pdata_raw(stack_top);
        pdata_free(stack_top);
      }
    }
  }

  stack_free(s);
}

void btree_walk(
    const btree_t *t, const btree_walk_mode mode, const btree_walk_func func) {
  if (t == NULL || t->root == NULL || func == NULL)
    return;

  switch (mode) {
  case INORDER:
    _btree_walk_inorder(t, func);
    break;
  case PREORDER:
    _btree_walk_preorder(t, func);
    break;
  case POSTORDER:
    _btree_walk_postorder(t, func);
    break;
  }
}

void btree_free(btree_t *t) {
  btree_walk(t, INORDER, btree_node_free);
  free(t);
}

int btree_is_empty(const btree_t *t) { return t == NULL || t->root == NULL; }

int btree_contains(const btree_t *t, const pdata *data) {
  int found = 0;
  if (t == NULL || t->root == NULL || t->cmp == NULL) {
    return found;
  }

  stack_t *s = stack_alloc();
  stack_push(s, pdata_from_ref(t->root));
  while (!stack_is_empty(s)) {
    const pdata *top = stack_peek(s);
    const btree_node_t *current = pdata_raw(top);
    pdata_free(top);
    if (t->cmp(current->payload, data) == 0) {
      found = 1;
      break;
    }
    if (current->right != NULL) {
      stack_push(s, pdata_from_ref(current->right));
    }
    if (current->left != NULL) {
      stack_push(s, pdata_from_ref(current->left));
    }
  }

  while (!stack_is_empty(s)) {
    pdata_free(stack_pop(s));
  }
  stack_free(s);

  return found;
}

const pdata *btree_find_min(const btree_t *t) {
  if (t == NULL || t->root == NULL) {
    return NULL;
  }
  const btree_node_t *current = t->root;
  while (current->left != NULL) {
    current = current->left;
  }
  return current->payload;
}

const pdata *btree_find_max(const btree_t *t) {
  if (t == NULL || t->root == NULL) {
    return NULL;
  }
  const btree_node_t *current = t->root;
  while (current->right != NULL) {
    current = current->right;
  }
  return current->payload;
}

int btree_insert(const btree_t *t, const pdata *data) {
  if (t == NULL || t->root == NULL || t->cmp == NULL) {
    return -1;
  }
  btree_node_t *current = t->root;
  while (1) {
    const int cmp = t->cmp(data, current->payload);
    if (cmp < 0) {
      if (current->left == NULL) {
        current->left = btree_node_with(data);
        current->left->parent = current;
        break;
      }
      current = current->left;
    } else if (cmp > 0) {
      if (current->right == NULL) {
        current->right = btree_node_with(data);
        current->right->parent = current;
        break;
      }
      current = current->right;
    } else {
      // duplicate
      return -1;
    }
  }

  return 0;
}

int btree_remove(const btree_t *t, const pdata *data) {
  if (t == NULL || t->root == NULL) {
    return -1;
  }

  return 0;
}
