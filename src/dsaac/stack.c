#include "stack.h"

stack_node_t *stack_top(const stack_t *s) {
  if (s && s->first) {
    return s->first;
  }
  return NULL;
}

stack_node_t *stack_pop(stack_t *s) {
  stack_node_t *node = s->first;
  s->first = s->first->next;
  s->length--;

  if (s->first == NULL)
    s->last = NULL;

  return node;
}

void stack_print(const stack_t *s) {
  const stack_node_t *node = s->first;

  DBGLog("**********************\n");
  DBGLog("Stack  : %p\n", (void *)s);
  DBGLog("length : %d\n", s->length);
  DBGLog("Top    : %p\n", (void *)s->first);
  DBGLog("Bottom : %p\n", (void *)s->last);
  DBGLog("-------------\n");

  while (node != NULL) {
    stack_node_print(node);

    node = node->next;
  }

  DBGLog("**********************\n");
}

void Stack_Test(void) {
  const char *payload3 = "this is a stack";
  stack_t *s = NULL;
  stack_node_t *node1 = NULL;
  stack_node_t *node2 = NULL;
  stack_node_t *node3 = NULL;

  node1 = stack_node_with(pdata_from_u32(1), 1);
  node2 = stack_node_with(pdata_from_u8('s'), 1);
  node3 = stack_node_with(pdata_from_string(payload3, 0), 1);

  s = stack_alloc();

  stack_push(s, node1);
  stack_push(s, node2);
  stack_push(s, node3);

  node1 = stack_pop(s);
  stack_node_print(node1);

  stack_node_free(node1);

  stack_print(s);

  stack_free(s);
}
