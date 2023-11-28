#include "tree.h"

/*
 * ************************************************************
 * tree_node_t
 * ************************************************************
 */

tree_node_t *tree_node_with(const pdata *payload, const int move) {
  tree_node_t *node = malloc(sizeof(tree_node_t));
  node->parent = NULL;
  node->sibling = NULL;
  node->child = NULL;
  node->own_pdata = move;
  node->data = payload;

  return node;
}

void tree_node_free(tree_node_t *node) {
  if (node && node->own_pdata) {
    pdata_free(node->data);
  }

  free(node);
}

void tree_node_print(tree_node_t *node) {
  DBGLog("%p | ", (void *)node);
  if (node == NULL) {
    printf("null \n");
    return;
  }

  pdata_print(node->data);
}

/*
 * ************************************************************
 * tree_t
 * ************************************************************
 */

tree_t *tree_alloc(void) {
  tree_t *ret = malloc(sizeof(tree_t));
  memset(ret, 0, sizeof(tree_t));

  ret->root = NULL;

  return ret;
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
void tree_free(tree_t *t) {
  list_t *l = NULL;
  const list_node_t *dnode = NULL;

  /*
   * 1  1st exploration of node,  continue with 1st subnode
   * 0 node completely explored, continue with sibling
   */

  /* the current node during exploration */
  tree_node_t *current = NULL;

  if (t->root == NULL)
    return;

  /*
   * ----- the root pre order
   * (* process) (root, TRUE);
   */

  if (t->root) {
    int dirFlag = 1;
    l = list_alloc();
    list_push(l, list_node_with(pdata_from_ref(t->root), 1));
    /* ----- the exploration loop */
    current = t->root->child;
    while ((current != NULL) && (current != t->root || dirFlag)) {
      /*
       * --- process the node : TRUE prefix, FALSE postfix
       * (* process) (current, dir_flag);
       */

      /*
       * --- 1st expl. of the node, continue with 1st subnode
       * or stay for 2nd part of the process
       */
      if (dirFlag) {
        /* add node to delete list */
        list_push(l, list_node_with(pdata_from_ref(current), 1));

        if (current->child != NULL)
          current = current->child;
        else
          /*
           * 1st exploration of current node is finished
           * go to 2nd exploration of the current node
           */
          dirFlag = 0;
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

          dirFlag = 1;
        } else
          current = current->parent;
      }
    }
    /* ASSERT ((current == root) && (dir_flag == FALSE)); */

    for (dnode = l->first; dnode != NULL; dnode = dnode->next) {
      tree_node_print(dnode->payload->data.p);
      tree_node_free(dnode->payload->data.p);
    }

    list_free(l);

    free(t);
  }

  /*
   * ----- the root post order
   * (* process) (root, FALSE);
   */
}

int tree_is_empty(const tree_t *t) { return t->count == 0; }

void tree_insert(tree_t *t, tree_node_t *parent, tree_node_t *node) {
  tree_node_t *current = NULL;

  t->count++;

  node->parent = parent;

  /* add node as the root of the tree */
  if (parent == NULL) {
    t->root = node;
    return;
  }

  /* create new branch */
  if (parent->child == NULL) {
    parent->child = node;
    return;
  }

  /* add to existed branch */
  current = parent->child;

  while (current->sibling != NULL)
    current = current->sibling;

  current->sibling = node;
}

/* test */
void tree_test(void) {
  tree_t *t = NULL;
  tree_node_t *root = NULL;
  tree_node_t *leaf_0_0 = NULL;
  tree_node_t *leaf_0_1 = NULL;
  tree_node_t *leaf_0_2 = NULL;
  tree_node_t *leaf_1_0 = NULL;
  tree_node_t *leaf_1_1 = NULL;
  tree_node_t *leaf_2_0 = NULL;

  const char *payload0 = "root";
  const char *payload1 = "leaf_0_0";
  const char *payload2 = "leaf_0_1";
  const char *payload3 = "leaf_0_2";
  const char *payload4 = "leaf_1_0";
  const char *payload5 = "leaf_1_1";
  const char *payload6 = "leaf_2_0";

  root = tree_node_with(pdata_from_string(payload0, 0), 1);
  leaf_0_0 = tree_node_with(pdata_from_string(payload1, 0), 1);
  leaf_0_1 = tree_node_with(pdata_from_string(payload2, 0), 1);
  leaf_0_2 = tree_node_with(pdata_from_string(payload3, 0), 1);
  leaf_1_0 = tree_node_with(pdata_from_string(payload4, 0), 1);
  leaf_1_1 = tree_node_with(pdata_from_string(payload5, 0), 1);
  leaf_2_0 = tree_node_with(pdata_from_string(payload6, 0), 1);

  t = tree_alloc();

  tree_insert(t, t->root, root);
  tree_insert(t, t->root, leaf_0_0);
  tree_insert(t, t->root, leaf_0_1);
  tree_insert(t, t->root, leaf_0_2);
  tree_insert(t, leaf_0_1, leaf_1_0);
  tree_insert(t, leaf_0_1, leaf_1_1);
  tree_insert(t, leaf_1_0, leaf_2_0);

  tree_free(t);
}
