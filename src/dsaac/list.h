#ifndef LIST_H_
#define LIST_H_

#include "common.h"
#include "payload.h"

#define list_first(l) (l)->first
#define list_last(l) (l)->last
#define list_len(l) (l)->length

/*
 * ************************************************************
 * list_node_t
 * ************************************************************
 */

typedef struct list_node_t {
  struct list_node_t *next;
  int owned;
  const pdata *payload;

} list_node_t;

list_node_t *list_node_with(const pdata *payload, int move);

void list_node_free(const list_node_t *node);

void list_node_print(const list_node_t *node);

/*
 * ************************************************************
 * list_t
 * ************************************************************
 */

typedef struct list_t {
  int length;
  list_node_t *last;
  list_node_t *first;

} list_t;

list_t *list_alloc(void);

void list_free(list_t *l);

int list_is_empty(const list_t *l);

list_node_t *list_prev(list_t *l, const list_node_t *node);

list_node_t *
list_insert(list_t *l, const list_node_t *where, list_node_t *node);

list_node_t *list_push(list_t *l, list_node_t *node);

list_node_t *list_unshift(list_t *l, list_node_t *node);

int list_remove(list_t *l, const list_node_t *node, int free);

list_node_t *list_locate(const list_t *l, int index);

void list_print(list_t *l);

/* test */
void list_test(void);

#endif /* LIST_H_ */
