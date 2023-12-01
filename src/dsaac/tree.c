#include "tree.h"
#include "list.h"

#include <stack.h>

tree_t *tree_alloc(const pdata *data) {
  tree_t *ret = malloc(sizeof(tree_t));

  ret->parent = NULL;
  ret->sibling = NULL;
  ret->child = NULL;
  ret->data = data;

  return ret;
}

void _tree_free_impl(tree_t *node) { free(node); }

void tree_free(tree_t *root) { tree_foreach(root, _tree_free_impl); }

void tree_node_visitor_from_list_node(void *func, const pdata *data) {
  if (data == NULL || func == NULL) {
    return;
  }
  tree_t *t = pdata_raw(data);
  ((void (*)(tree_t *))func)(t);
}

/*
 There are only two cases for the algorithm to consider. The first is when it is
 positioned at the top of an unexplored subtree; then it should visit the first
 subnode. The second case is when the algorithm has finished visiting a subtree;
 then it should visit either a sibling or the parent. The proper course of
 action can be deduced from the current position and the last operation
 performed. The algorithm does not need to store any information as it traverses
 the tree except for one bit.

 Figure 2 shows the C code for the core algorithm. (Full source code is
 available on the CUJ website, www.cuj.com/code.) Function tree_depth_traversal1
 implements either prefix- or postfix-order traversal (or both). It consists of
 a simple loop (after a few preliminary if statements) that operates on the node
 passed into the function. The two cases mentioned above are represented by the
 Boolean variable dir_flag.

 When dir_flag is TRUE, the algorithm must explore a subtree. The algorithm
 attempts to move to the first subnode (leaving dir_flag with the value TRUE
 because it's a new unexplored subtree). If the current node turns out to be a
 leaf the algorithm considers the subtree explored and sets dir_flag to FALSE.

 When dir_flag is FALSE, the algorithm attempts to move to a sibling (and sets
 dir_flag to TRUE because the sibling is a new unexplored subtree). If there is
 no sibling the algorithm moves to the parent (and leaves dir_flag FALSE because
 the subtree that has the parent as its root is completely explored).

 The algorithm begins the traversal with a node and a user-supplied function to
 process each node. The initial value of the flag is obviously TRUE.

 The user-supplied function for processing a node controls whether the traversal
 is prefix-order, post-fix order, or both. The second parameter to the function,
 first_flag, is a Boolean that indicates whether the function is being called
 before the node's subtree has been visited, or after. To effect a prefix
 traversal, define the function to work only when first_flag is TRUE, and to do
 nothing when first_flag is FALSE. To effect a postfix traversal, define the
 function to work only when first_flag is FALSE. To get a combined
 prefix/postfix traversal, define the function to ignore first_flag.
 */
void tree_foreach(tree_t *root, void (*func)(tree_t *node)) {
  if (root == NULL) {
    return;
  }

  list_t *l = NULL;

  /*
   * 1  1st exploration of node,  continue with 1st subnode
   * 0 node completely explored, continue with sibling
   */

  /* the current node during exploration */
  tree_t *current = NULL;

  /*
   * ----- the root pre order
   * (* process) (root, TRUE);
   */

  int dir_flag = 1;
  l = list_alloc();
  list_push(l, pdata_from_ref(root));
  /* ----- the exploration loop */
  current = root->child;
  while ((current != NULL) && (current != root || dir_flag)) {
    /*
     * --- process the node : TRUE prefix, FALSE postfix
     * (* process) (current, dir_flag);
     */

    /*
     * --- 1st expl. of the node, continue with 1st subnode
     * or stay for 2nd part of the process
     */
    if (dir_flag) {
      /* add node to delete list */
      list_push(l, pdata_from_ref(current));

      if (current->child != NULL)
        current = current->child;
      else
        /*
         * 1st exploration of current node is finished
         * go to 2nd exploration of the current node
         */
        dir_flag = 0;
    }

    /*
     * --- second expl. of the node, continue with sibling
     * or the parent if no sibling
     */
    else {
      if (current->sibling != NULL) {
        /*
         * goto 1st exploration of the sibling (now current)
         */
        current = current->sibling;

        dir_flag = 1;
      } else
        current = current->parent;
    }
  }
  /* ASSERT ((current == root) && (dir_flag == FALSE)); */
  list_foreach_ctx(l, (void *)func, tree_node_visitor_from_list_node);
  list_foreach(l, pdata_free);
  list_free(l);

  /*
   * ----- the root post order
   * (* process) (root, FALSE);
   */
}

#define DOT_HEADER                                                             \
  "digraph g {\
fontname=\"Helvetica,Arial,sans-serif\"\n\
node [fontname=\"Helvetica,Arial,sans-serif\"]\n\
edge [fontname=\"Helvetica,Arial,sans-serif\"]\n\
node [fontsize = \"16\", shape = \"ellipse\"];\n\
edge [];\n"

#define DOT_TAIL "}"

const pdata *_tree_node_print(const tree_t *t) {
  char *data_buf = NULL;
  size_t data_size = 0;
  pdata_print(t->data, &data_buf, &data_size);

  char node_buf[1024];
  const size_t node_size = 1024;
  memset(node_buf, 0, node_size);

  snprintf(
      node_buf, node_size,
      "\"%p\" [\nlabel = \"<f0> %p | %s\"\nshape = \"record\"\n];\n", t, t,
      data_buf);

  free(data_buf);

  const size_t slen = strlen(node_buf);
  char *buf = malloc(slen + 1);
  strcpy(buf, node_buf);
  buf[slen] = '\0';

  return pdata_from_str(buf);
}

const pdata *
_tree_node_print_edge(const tree_t *from, const tree_t *to, const int id) {
  char buf[1024];
  const size_t size = 1024;
  snprintf(buf, size, "\"%p\":f0 -> \"%p\":f0 [\nid=%d\n];\n", from, to, id);

  const size_t slen = strlen(buf);
  char *buf2 = malloc(slen + 1);
  strcpy(buf2, buf);
  buf2[slen] = '\0';

  return pdata_from_str(buf2);
}

ssize_t tree_to_dot(tree_t *root, char **buf, size_t *size) {
  if (root == NULL || buf == NULL || size == NULL) {
    return -1;
  }

  ssize_t total_size = 0;
  list_t *head = list_alloc();
  list_t *tail = list_alloc();

  char *header = malloc(strlen(DOT_HEADER));
  strcpy(header, DOT_HEADER);

  list_push(head, pdata_from_str(header));

  stack_t *s = stack_alloc();
  stack_push(s, pdata_from_ref(root));
  while (!stack_is_empty(s)) {
    const pdata *d = stack_pop(s);
    const tree_t *t = pdata_raw(d);
    pdata_free(d);

    list_push(head, _tree_node_print(t));
    if (t->parent != NULL) {
      list_push(tail, _tree_node_print_edge(t->parent, t, list_len(tail)));
    }

    tree_t *child = t->child;
    while (child != NULL) {
      stack_push(s, pdata_from_ref(child));
      child = child->sibling;
    }
  }
  stack_free(s);

  char *end = malloc(strlen(DOT_TAIL));
  strcpy(end, DOT_TAIL);
  list_push(tail, pdata_from_str(end));

  // calculate total size
  list_iter_t *head_iter = list_iter_alloc(head);
  const pdata *d = NULL;
  while ((d = list_iter_next(head_iter)) != NULL) {
    total_size += pdata_size(d);
  }

  list_iter_t *tail_iter = list_iter_alloc(tail);
  while ((d = list_iter_next(tail_iter)) != NULL) {
    total_size += pdata_size(d);
  }

  total_size++;

  // allocate buffer if needed
  if (*buf == NULL) {
    *size = total_size;
    *buf = malloc(total_size);
    if (*buf == NULL) {
      total_size = -1;
    }
  } else {
    if (*size < total_size) {
      total_size = -1;
    }
  }

  // actual print
  size_t pos = 0;
  list_iter_reset(head_iter);
  while ((d = list_iter_next(head_iter)) != NULL) {
    if (total_size > 0) {
      memcpy(*buf + pos, pdata_raw(d), pdata_size(d));
      pos += pdata_size(d);
    }
    free(pdata_raw(d));
  }
  list_iter_free(head_iter);

  list_iter_reset(tail_iter);
  while ((d = list_iter_next(tail_iter)) != NULL) {
    if (total_size > 0) {
      memcpy(*buf + pos, pdata_raw(d), pdata_size(d));
      pos += pdata_size(d);
    }
    free(pdata_raw(d));
  }
  list_iter_free(tail_iter);
  if (total_size > 0) {
    // the tailing '\0'
    (*buf)[pos] = '\0';
  }

  // free lists
  list_foreach(head, pdata_free);
  list_foreach(tail, pdata_free);
  list_free(head);
  list_free(tail);

  if (total_size > 0) {
    *size = total_size;
  }

  return total_size;
}

/**
*digraph g {
fontname="Helvetica,Arial,sans-serif"
node [fontname="Helvetica,Arial,sans-serif"]
edge [fontname="Helvetica,Arial,sans-serif"]
node [fontsize = "16", shape = "ellipse"];
edge [];
"node0" [
label = "<f0> 0x10ba8 | root"
shape = "record"
];
"node1" [
label = "<f0> 0xf7fc4380 | node1"
shape = "record"
];
"node2" [
label = "<f0> 0xf7fc4388 | node1-1"
shape = "record"
];
"node3" [
label = "<f0> 0xf7fc4390 | node2"
shape = "record"
];
"node0":f0 -> "node1":f0 [
id = 0
];
"node1":f0 -> "node2":f0 [
id = 1
];
"node1":f1 -> "node3":f0 [
id = 2
];
}
*/
