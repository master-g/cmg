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
  size_t length;
  list_node_t *last;
  list_node_t *first;
};

list_t *list_alloc(void) {
  list_t *ret = malloc(sizeof(list_t));

  ret->length = 0;
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

int list_insert(list_t *l, const int where, const pdata *data) {
  if (l == NULL || where < 0 || where > l->length) {
    return -1;
  }

  list_node_t *node = list_node_with(data);
  if (node == NULL) {
    return -1;
  }
  if (l->length == 0) {
    l->first = node;
    l->last = node;
  } else if (where == 0) {
    node->next = l->first;
    l->first->prev = node;
    l->first = node;
  } else if (where == l->length) {
    node->prev = l->last;
    l->last->next = node;
    l->last = node;
  } else {
    list_node_t *current = l->first;
    for (int i = 0; i < where; i++) {
      current = current->next;
    }
    node->prev = current->prev;
    node->next = current;
    current->prev->next = node;
    current->prev = node;
  }
  l->length++;

  return 1;
}

int list_push(list_t *l, const pdata *data) {
  if (l == NULL) {
    return -1;
  }
  list_node_t *node = list_node_with(data);
  if (node == NULL) {
    return -1;
  }

  if (l->length == 0) {
    l->first = node;
    l->last = node;
  } else {
    node->prev = l->last;
    l->last->next = node;
    l->last = node;
  }
  l->length++;

  return 1;
}

int list_unshift(list_t *l, const pdata *data) {
  if (l == NULL) {
    return -1;
  }
  list_node_t *node = list_node_with(data);
  if (node == NULL) {
    return -1;
  }

  if (l->length == 0) {
    l->first = node;
    l->last = node;
  } else {
    node->next = l->first;
    l->first->prev = node;
    l->first = node;
  }
  l->length++;

  return 1;
}

int list_remove(list_t *l, const pdata *data) {
  const list_node_t *current = l->first;
  while (current != NULL) {
    if (current->payload == data) {
      if (current->prev != NULL)
        current->prev->next = current->next;
      if (current->next != NULL)
        current->next->prev = current->prev;

      l->length--;
      list_node_free(current);
      return 1;
    }
    current = current->next;
  }
  return 0;
}

const pdata *list_remove_at(list_t *l, const int index) {
  if (l == NULL || index < 0 || index >= l->length)
    return NULL;

  int i = 0;
  for (const list_node_t *current = l->first; current != NULL;
       current = current->next) {
    if (index == i) {
      if (current->prev != NULL)
        current->prev->next = current->next;
      if (current->next != NULL)
        current->next->prev = current->prev;

      l->length--;
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
    l->length--;
    return ret;
  }

  const pdata *ret = l->first->payload;
  list_node_t *next = l->first->next;
  list_node_free(l->first);
  l->first = next;
  l->length--;

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
    l->length--;
    return ret;
  }

  const pdata *ret = l->last->payload;
  list_node_t *prev = l->last->prev;
  list_node_free(l->last);
  l->last = prev;
  l->length--;

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
  if (index < 0 || index >= l->length)
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
  if (l == NULL)
    return 0;
  return l->length;
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
