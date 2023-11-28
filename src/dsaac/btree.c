#include "btree.h"
#include "stack.h"

/*
 * ************************************************************
 * btree_node_t
 * ************************************************************
 */

btree_node_t *btree_node_with(const pdata *payload, const int move) {
  btree_node_t *node = malloc(sizeof(btree_node_t));
  memset(node, 0, sizeof(btree_node_t));
  if (node == NULL)
    return NULL;

  node->payload = payload;
  node->owned = move;

  return node;
}

void btree_node_free(const btree_node_t *node) {
  if (node && node->owned)
    pdata_free(node->payload);

  free((void *)node);
}

void btree_node_print(const btree_node_t *node) {
  DBGLog("%p | ", (void *)node);
  if (node == NULL) {
    printf("null \n");
    return;
  }

  pdata_print(node->payload);
}

/*
 * ************************************************************
 * btree_t
 * ************************************************************
 */

btree_t *btree_alloc(void) {
  btree_t *ret = malloc(sizeof(btree_t));
  memset(ret, 0, sizeof(btree_t));

  ret->root = NULL;

  return ret;
}

void _btree_walk_inorder(const btree_t *t, const btree_walk_func func) {
  stack_t *s = stack_alloc();

  btree_node_t *current = t->root;
  while (current != NULL || !stack_is_empty(s)) {
    // current and its left children
    while (current != NULL) {
      stack_push(s, stack_node_with(pdata_from_ref(current), 1));
      current = current->left;
    }
    // pop and visit
    const stack_node_t *sn = stack_pop(s);
    current = sn->payload->data.p;
    func(current);
    stack_node_free(sn);

    // handle right children
    current = current->right;
  }

  stack_free(s);
}

void _btree_walk_preorder(const btree_t *t, const btree_walk_func func) {
  stack_t *s = stack_alloc();

  btree_node_t *current = t->root;
  while (current != NULL || !stack_is_empty(s)) {
    if (current != NULL) {
      // visit
      func(current);
      // push
      stack_push(s, stack_node_with(pdata_from_ref(current), 1));
      // handle left children
      current = current->left;
    } else {
      // handle right children
      const stack_node_t *sn = stack_pop(s);
      current = sn->payload->data.p;
      stack_node_free(sn);

      // handle right children
      current = current->right;
    }
  }

  stack_free(s);
}

void _btree_walk_postorder(const btree_t *t, const btree_walk_func func) {
  stack_t *s = stack_alloc();

  btree_node_t *current = t->root;
  const btree_node_t *last_visited = NULL;

  while (current != NULL || !stack_is_empty(s)) {
    if (current != NULL) {
      // push
      stack_push(s, stack_node_with(pdata_from_ref(current), 1));
      // handle left children
      current = current->left;
    } else {
      btree_node_t *peek_node = stack_top(s)->payload->data.p;
      // check if right children has been already visited
      if (peek_node->right != NULL && last_visited != peek_node->right) {
        // handle right children
        current = peek_node->right;
      } else {
        // visit
        func(peek_node);
        // pop
        const stack_node_t *sn = stack_pop(s);
        last_visited = sn->payload->data.p;
        stack_node_free(sn);
      }
    }
  }

  stack_free(s);
}

void btree_walk(
    const btree_t *t, const btree_walk_mode_t mode,
    const btree_walk_func func) {
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

int btree_is_empty(const btree_t *t) { return t->count == 0; }

btree_node_t *
btree_find(const btree_t *t, const pdata *payload, const pdata_cmp_func cmp) {
  if (t == NULL || t->root == NULL) {
    return NULL;
  }

  stack_t *s = stack_alloc();
  stack_push(s, stack_node_with(pdata_from_ref(t->root), 1));
  btree_node_t *found = NULL;
  while (!stack_is_empty(s)) {
    const stack_node_t *sn = stack_top(s);
    btree_node_t *current = sn->payload->data.p;
    stack_node_free(sn);
    if (cmp(current->payload, payload) == 0) {
      found = current;
      break;
    }
    if (current->right != NULL) {
      stack_push(s, stack_node_with(pdata_from_ref(current->right), 1));
    }
    if (current->left != NULL) {
      stack_push(s, stack_node_with(pdata_from_ref(current->left), 1));
    }
  }

  stack_free(s);

  return found;
}

btree_node_t *btree_find_min(btree_t *t, pdata_cmp_func cmp) { return NULL; }

btree_node_t *btree_find_max(btree_t *t, pdata_cmp_func cmp) { return NULL; }
