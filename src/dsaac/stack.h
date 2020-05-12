#ifndef STACK_H_
#define STACK_H_

#include "list.h"

/*
 * ************************************************************
 * stack_node_t
 * ************************************************************
 */

typedef list_node_t stack_node_t;

#define StackNode_Create ListNode_Create
#define StackNode_Destroy(n) ListNode_Destroy(n)
#define StackNode_Print(n) ListNode_Print(n)

/*
 * ************************************************************
 * stack_t
 * ************************************************************
 */

#define stack_t list_t

#define Stack_Create List_Create
#define Stack_Destroy(s) List_Destroy(s)
#define Stack_IsEmpty(s) List_IsEmpty(s)
#define Stack_Length(s) List_Length(s)
#define Stack_Push(s, n) List_PushFront(s, n);
#define Stack_Top(s) (s)->first

#define Stack_PushInt(s, d) List_PushFront_Int(s, d)
#define Stack_PushChar(s, d) List_PushFront_Char(s, d)
#define Stack_PushString(s, d) List_PushFront_String(s, d)

stack_node_t *Stack_Pop(stack_t *s);

void Stack_Print(stack_t *s);

/* test */
void Stack_Test(void);

#endif /* STACK_H_ */