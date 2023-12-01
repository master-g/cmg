/*
 *  list.c
 *  DSAAC
 *
 *  Created by Master.G on 23-11-30.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */
#include "list.h"
#include "common.h"

typedef struct list_node_t {
  struct list_node_t *prev;
  struct list_node_t *next;
  const pdata *payload;
} list_node_t;

list_node_t *list_node_with(const pdata *payload) {
  list_node_t *node = malloc(sizeof(list_node_t));
  node->prev = NULL;
  node->next = NULL;
  node->payload = payload;

  return node;
}

void list_node_free(const list_node_t *node) { free((void *)node); }

/*
 * ************************************************************
 * list_t
 * ************************************************************
 */

struct list_t {
  list_node_t *last;
  list_node_t *first;
};

/*
 * ************************************************************
 * iter
 * ************************************************************
 */

struct list_iter_t {
  const list_t *l;
  int where; // -1 for before first, 0 for in list, 1 for beyond list
  list_node_t *current;
};

#define LIST_ITER_AHEAD -1
#define LIST_ITER_INSIDE 0
#define LIST_ITER_BEYOND 1

list_iter_t *list_iter_alloc(const list_t *l) {
  list_iter_t *iter = malloc(sizeof(list_iter_t));
  iter->l = l;
  iter->where = LIST_ITER_AHEAD;
  return iter;
}

const pdata *list_iter_next(list_iter_t *iter) {
  if (iter == NULL || iter->l == NULL) {
    return NULL;
  }
  if (iter->where == LIST_ITER_AHEAD) {
    iter->where = LIST_ITER_INSIDE;
    iter->current = iter->l->first;
    return iter->current ? iter->current->payload : NULL;
  }

  if (iter->where == LIST_ITER_BEYOND) {
    return NULL;
  }

  if (iter->current) {
    iter->current = iter->current->next;
    if (iter->current) {
      return iter->current->payload;
    }
    iter->where = LIST_ITER_BEYOND;
  }

  return NULL;
}

const pdata *list_iter_prev(list_iter_t *iter) {
  if (iter == NULL || iter->l == NULL) {
    return NULL;
  }

  if (iter->where == LIST_ITER_BEYOND) {
    iter->where = LIST_ITER_INSIDE;
    iter->current = iter->l->last;
    return iter->current ? iter->current->payload : NULL;
  }

  if (iter->where == LIST_ITER_AHEAD) {
    return NULL;
  }

  if (iter->current) {
    iter->current = iter->current->prev;
    if (iter->current) {
      return iter->current->payload;
    }
    iter->where = LIST_ITER_AHEAD;
  }

  return NULL;
}

void list_iter_reset(list_iter_t *iter) {
  if (iter == NULL || iter->l == NULL) {
    return;
  }
  iter->where = LIST_ITER_AHEAD;
  iter->current = iter->l->first;
}

void list_iter_free(list_iter_t *iter) { free(iter); }

/*
 * ************************************************************
 * list
 * ************************************************************
 */

list_t *list_alloc(void) {
  list_t *ret = malloc(sizeof(list_t));

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

int list_is_empty(const list_t *l) {
  if (l == NULL || l->first == NULL || l->last == NULL) {
    return 1;
  }

  return 0;
}

int list_insert(list_t *l, const int where, const pdata *data) {
  if (l == NULL || where < 0) {
    return -1;
  }

  if (where == 0) {
    return list_unshift(l, data);
  }

  const size_t len = list_len(l);
  if (where > len) {
    return -1;
  }

  if (where == len) {
    return list_push(l, data);
  }

  list_node_t *node = list_node_with(data);
  if (node == NULL) {
    return -1;
  }

  int i = 0;
  for (list_node_t *current = l->first; current != NULL;
       current = current->next) {
    if (i == where) {
      node->prev = current->prev;
      node->next = current;
      current->prev->next = node;
      current->prev = node;
      return 0;
    }
    i++;
  }

  return -1;
}

int list_push(list_t *l, const pdata *data) {
  if (l == NULL) {
    return -1;
  }
  list_node_t *node = list_node_with(data);
  if (node == NULL) {
    return -1;
  }

  if (l->first == NULL) {
    l->first = node;
    l->last = node;
  } else {
    node->prev = l->last;
    l->last->next = node;
    l->last = node;
  }

  return 0;
}

int list_unshift(list_t *l, const pdata *data) {
  if (l == NULL) {
    return -1;
  }
  list_node_t *node = list_node_with(data);
  if (node == NULL) {
    return -1;
  }

  if (l->first == NULL) {
    l->first = node;
    l->last = node;
  } else {
    node->next = l->first;
    l->first->prev = node;
    l->first = node;
  }

  return 0;
}

int list_remove(const list_t *l, const pdata *data) {
  const list_node_t *current = l->first;
  while (current != NULL) {
    if (current->payload == data) {
      if (current->prev != NULL)
        current->prev->next = current->next;
      if (current->next != NULL)
        current->next->prev = current->prev;

      list_node_free(current);
      return 0;
    }
    current = current->next;
  }
  return -1;
}

const pdata *list_remove_at(const list_t *l, const int index) {
  if (list_is_empty(l)) {
    return NULL;
  }

  int i = 0;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    if (index == i) {
      if (current->prev != NULL)
        current->prev->next = current->next;
      if (current->next != NULL)
        current->next->prev = current->prev;

      const pdata *ret = current->payload;
      list_node_free(current);
      return ret;
    }
    i++;
  }

  return NULL;
}

const pdata *list_remove_first(list_t *l) {
  if (l->first == NULL)
    return NULL;

  if (l->first == l->last) {
    const pdata *ret = l->first->payload;
    list_node_free(l->first);
    l->first = NULL;
    l->last = NULL;
    return ret;
  }

  const pdata *ret = l->first->payload;
  list_node_t *next = l->first->next;
  list_node_free(l->first);
  l->first = next;

  return ret;
}

const pdata *list_remove_last(list_t *l) {
  if (l->last == NULL)
    return NULL;

  if (l->first == l->last) {
    const pdata *ret = l->last->payload;
    list_node_free(l->last);
    l->first = NULL;
    l->last = NULL;
    return ret;
  }

  const pdata *ret = l->last->payload;
  list_node_t *prev = l->last->prev;
  list_node_free(l->last);
  l->last = prev;

  return ret;
}

const pdata *list_first(const list_t *l) {
  if (l->first == NULL)
    return NULL;
  return l->first->payload;
}

const pdata *list_last(const list_t *l) {
  if (l->last == NULL)
    return NULL;
  return l->last->payload;
}

const pdata *list_get(const list_t *l, int index) {
  if (l == NULL) {
    return NULL;
  }
  const size_t len = list_len(l);

  if (index < 0 || index >= len)
    return NULL;

  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    if (index == 0)
      return current->payload;
    index--;
  }
  return NULL;
}

size_t list_len(const list_t *l) {
  if (l == NULL || l->first == NULL)
    return 0;
  size_t len = 0;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    len++;
  }
  return len;
}

int list_index_of(const list_t *l, const pdata *data) {
  if (l == NULL)
    return -1;

  int i = 0;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    if (current->payload == data) {
      return i;
    }
    i++;
  }
  return -1;
}

int list_find(const list_t *l, const list_find_func finder, const pdata *key) {
  if (l == NULL || finder == NULL)
    return -1;

  int i = 0;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    if (finder(current->payload, key) == 0) {
      return i;
    }
    i++;
  }
  return -1;
}

void list_foreach_self_ctx_indexed(
    const list_t *l, void *ctx,
    void (*func)(const list_t *l_, void *ctx_, int index, const pdata *data)) {
  if (l == NULL || func == NULL)
    return;
  int i = 0;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    func(l, ctx, i, current->payload);
    i++;
  }
}

void list_foreach_ctx_indexed(
    const list_t *l, void *ctx,
    void (*func)(void *ctx_, int index, const pdata *data)) {
  if (l == NULL || func == NULL)
    return;
  int i = 0;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    func(ctx, i, current->payload);
    i++;
  }
}

void list_foreach_self_indexed(
    const list_t *l, void *ctx,
    void (*func)(const list_t *l_, int index, const pdata *data)) {
  if (l == NULL || func == NULL)
    return;

  int i = 0;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    func(l, i, current->payload);
    i++;
  }
}

void list_foreach_self_ctx(
    const list_t *l, void *ctx,
    void (*func)(const list_t *l_, void *ctx_, const pdata *data)) {
  if (l == NULL || func == NULL)
    return;

  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    func(l, ctx, current->payload);
  }
}

void list_foreach_self(
    const list_t *l, void (*func)(const list_t *l_, const pdata *data)) {
  if (l == NULL || func == NULL)
    return;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    func(l, current->payload);
  }
}

void list_foreach_ctx(
    const list_t *l, void *ctx, void (*func)(void *ctx_, const pdata *data)) {
  if (l == NULL || func == NULL)
    return;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    func(ctx, current->payload);
  }
}

void list_foreach_indexed(
    const list_t *l, void (*func)(int index, const pdata *data)) {
  if (l == NULL || func == NULL)
    return;
  int i = 0;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    func(i, current->payload);
    i++;
  }
}

void list_foreach(const list_t *l, void (*func)(const pdata *)) {
  if (l == NULL || func == NULL)
    return;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    func(current->payload);
  }
}
