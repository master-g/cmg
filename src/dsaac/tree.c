#include "tree.h"
#include "dot.h"
#include "list.h"
#include "stack.h"

tree_t *tree_alloc(const pdata *data) {
  tree_t *ret = malloc(sizeof(tree_t));

  ret->parent = NULL;
  ret->sibling = NULL;
  ret->child = NULL;
  ret->data = data;

  return ret;
}

void _tree_free_impl(const tree_t *node) { free((void *)node); }

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
void tree_foreach(tree_t *root, void (*func)(const tree_t *node)) {
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

void _tree_node_print(dot_t *dot, const tree_t *t) {
  char *data_buf = NULL;
  size_t data_size = 0;
  pdata_print(t->data, &data_buf, &data_size);

  char node_buf[1024];
  const size_t node_size = 1024;
  memset(node_buf, 0, node_size);

  snprintf(
      node_buf, node_size,
      "\"%p\" [\n\tlabel = \"%p | %s\"\n\tshape = \"record\"\n];\n", t, t,
      data_buf);

  free(data_buf);

  dot_push_body(dot, node_buf);
}

void _tree_node_print_edge(
    dot_t *dot, const tree_t *from, const tree_t *to, const int id) {

  char buf[128];
  const size_t size = 128;
  memset(buf, 0, size);

  snprintf(buf, size, "\"%p\" -> \"%p\" [\n\tid=%d\n];\n", from, to, id);

  dot_push_foot(dot, buf);
}

ssize_t tree_to_dot(tree_t *root, char **buf, size_t *size) {
  if (root == NULL || buf == NULL || size == NULL) {
    return -1;
  }

  dot_t *dot = dot_alloc();

  ssize_t total_size = 0;

  stack_t *s = stack_alloc();
  stack_push(s, pdata_from_ref(root));
  int edge = 0;
  while (!stack_is_empty(s)) {
    const pdata *d = stack_pop(s);
    const tree_t *t = pdata_raw(d);
    pdata_free(d);

    _tree_node_print(dot, t);
    if (t->parent != NULL) {
      _tree_node_print_edge(dot, t->parent, t, edge);
      edge++;
    }

    tree_t *child = t->child;
    while (child != NULL) {
      list_unshift(s, pdata_from_ref(child));
      child = child->sibling;
    }
  }
  stack_free(s);

  total_size = dot_dump(dot, buf, size);
  dot_free(dot);

  return total_size;
}
