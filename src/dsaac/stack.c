#include "stack.h"

stack_node_t *Stack_Pop(stack_t *s) {
  stack_node_t *node = s->first;
  s->first = s->first->next;
  s->length--;

  if (s->first == NULL)
    s->last = NULL;

  return node;
}

void Stack_Print(stack_t *s) {
  stack_node_t *node = s->first;

  DBGLog("**********************\n");
  DBGLog("Stack  : %p\n", (void *)s);
  DBGLog("length : %d\n", s->length);
  DBGLog("Top    : %p\n", (void *)s->first);
  DBGLog("Bottom : %p\n", (void *)s->last);
  DBGLog("-------------\n");

  while (node != NULL) {
    StackNode_Print(node);

    node = node->next;
  }

  DBGLog("**********************\n");
}

void Stack_Test(void) {
  int payload1 = 1;
  char payload2 = 's';
  char *payload3 = "this is a stack";
  stack_t *s = NULL;
  stack_node_t *node1 = NULL;
  stack_node_t *node2 = NULL;
  stack_node_t *node3 = NULL;

  node1 = StackNode_Create();
  node1->payload = Payload_CreateWithInt(payload1);
  node2 = StackNode_Create();
  node2->payload = Payload_CreateWithChar(payload2);
  node3 = StackNode_Create();
  node3->payload = Payload_CreateWithString(payload3);

  s = Stack_Create();

  Stack_Push(s, node1);
  Stack_Push(s, node2);
  Stack_Push(s, node3);

  Stack_PushChar(s, 'a');

  node1 = Stack_Pop(s);
  StackNode_Print(node1);

  StackNode_Destroy(node1);

  Stack_Print(s);

  Stack_Destroy(s);
}