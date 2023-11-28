#include "list.h"

/*
 * ************************************************************
 * list_node_t
 * ************************************************************
 */

list_node_t *list_node_with(const pdata *payload, const int move) {
  list_node_t *node = malloc(sizeof(list_node_t));
  memset(node, 0, sizeof(list_node_t));
  node->owned = move;
  node->payload = payload;

  return node;
}

void list_node_free(const list_node_t *node) {
  if (node->payload != NULL && node->owned)
    pdata_free(node->payload);

  free((void *)node);
}

void list_node_print(const list_node_t *node) {
  DBGLog("%p | ", (void *)node);
  if (node == NULL) {
    printf("null \n");
    return;
  }

  pdata_print(node->payload);
}

/*
 * ************************************************************
 * list_t
 * ************************************************************
 */

list_t *list_alloc(void) {
  list_t *ret = malloc(sizeof(list_t));
  memset(ret, 0, sizeof(list_t));

  ret->first = NULL;
  ret->last = NULL;

  return ret;
}

void list_free(list_t *l) {
  const list_node_t *prev = NULL;
  const list_node_t *node = NULL;

  for (node = l->first; node != NULL;) {
    prev = node;
    node = node->next;
    list_node_free(prev);
  }

  free(l);
}

int list_is_empty(const list_t *l) { return l->length == 0; }

list_node_t *list_prev(list_t *l, const list_node_t *node) {
  list_node_t **pprev = NULL;

  for (pprev = &(l->first); *pprev != NULL; pprev = &(*pprev)->next) {
    if ((*pprev)->next == node)
      break;
  }

  return *pprev;
}

list_node_t *
list_insert(list_t *l, const list_node_t *where, list_node_t *node) {
  list_node_t *iter = l->first;

  /* inster front */
  if (where == l->first) {
    list_unshift(l, node);
    return node;
  }

  /* find position node in list */
  while (iter->next != where && iter->next != NULL)
    iter = iter->next;

  /* position node is not in the list */
  if (iter->next != where)
    return node;

  /* insert(list, NULL, node) */
  if (iter->next == NULL)
    l->last = node;

  node->next = iter->next;
  iter->next = node;

  l->length++;

  return node;
}

list_node_t *list_push(list_t *l, list_node_t *node) {
  list_node_t *last = l->last;

  l->length++;

  /* empty list */
  if (l->first == NULL) {
    l->first = node;
    l->last = node;
    node->next = NULL;
  } else {
    last->next = node;
    node->next = NULL;
    l->last = node;
  }

  return node;
}

list_node_t *list_unshift(list_t *l, list_node_t *node) {
  /* empty list */
  if (l->last == NULL) {
    l->last = node;
    l->last->next = NULL;
  }

  node->next = l->first;
  l->first = node;
  l->length++;

  return node;
}

int list_remove(list_t *l, const list_node_t *node, const int free) {
  list_node_t *prev = NULL;
  list_node_t *cur = l->first;

  if (node == l->first) {
    l->first = l->first->next;
  } else {
    while (cur != NULL && cur != node && cur->next != NULL) {
      prev = cur;
      cur = cur->next;
    }

    /* can't find node in list */
    if (cur == NULL || cur != node)
      return 0;

    /* remove last node */
    if (cur == l->last)
      l->last = prev;

    prev->next = cur->next;
  }

  l->length--;

  if (free)
    list_node_free(cur);

  return 1;
}

list_node_t *list_locate(const list_t *l, const int index) {
  int i = 0;
  list_node_t *ret = l->first;

  while (i != index) {
    ret = ret->next;
    i++;
  }

  return ret;
}

void list_print(list_t *l) {
  const list_node_t *node = l->first;

  DBGLog("**********************\n");
  DBGLog("List   : %p\n", (void *)l);
  DBGLog("length : %d\n", l->length);
  DBGLog("first  : %p\n", (void *)l->first);
  DBGLog("last   : %p\n", (void *)l->last);
  DBGLog("-------------\n");

  while (node != NULL) {
    list_node_print(node);

    node = node->next;
  }

  DBGLog("**********************\n");
}

/*
 * ************************************************************
 * test
 * ************************************************************
 */
void list_test(void) {
  list_node_t *first = NULL;
  list_node_t *second = NULL;
  list_node_t *third = NULL;
  list_node_t *fourth = NULL;

  list_t *l = list_alloc();

  first = list_node_with(pdata_from_u8(8), 1);
  list_unshift(l, first);

  second = list_node_with(pdata_from_u32(32), 1);
  list_push(l, second);

  third = list_node_with(pdata_from_string("hello world!", 0), 1);
  list_insert(l, NULL, third);

  fourth = list_node_with(pdata_from_float(3.14), 1);
  list_push(l, fourth);

  list_remove(l, first, 1);

  list_print(l);

  list_free(l);
}
