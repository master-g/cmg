#ifndef STACK_H_
#define STACK_H_

#include "list.h"

/*
 * ************************************************************
 * stack_node_t
 * ************************************************************
 */

typedef list_node_t stack_node_t;

#define stack_node_with list_node_with
#define stack_node_free list_node_free
#define stack_node_print list_node_print

/*
 * ************************************************************
 * stack_t
 * ************************************************************
 */

#define stack_t list_t

#define stack_alloc list_alloc
#define stack_free list_free
#define stack_is_empty list_is_empty
#define stack_push list_push

stack_node_t *stack_top(const stack_t *s);
stack_node_t *stack_pop(stack_t *s);
void stack_print(const stack_t *s);

/* test */
void stack_test(void);

#endif /* STACK_H_ */
