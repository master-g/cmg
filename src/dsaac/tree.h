#ifndef TREE_H_
#define TREE_H_

#include "payload.h"

typedef struct tree_t {
  struct tree_t *parent;
  struct tree_t *sibling;
  struct tree_t *child;
  const pdata *data;
} tree_t;

tree_t *tree_alloc(const pdata *data);
void tree_free(tree_t *root);
void tree_foreach(tree_t *root, void (*func)(const tree_t *));
tree_t *tree_add_child(tree_t *root, const pdata *data);

ssize_t tree_to_dot(tree_t *root, char **buf, size_t *size);

#endif /* TREE_H_ */
