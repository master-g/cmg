#include "common.h"
#include "list.h"
#include "stack.h"

void Error(void) {
  printf("Invalid expression !\n");
  exit(0);
}

list_t *Convert_Infix2Posfix(const char *str) {
  size_t strOffset = 0;
  list_node_t *node = NULL;
  char c = 0;
  char cInStack = 0;
  stack_t *symbolStack = Stack_Create();
  list_t *outputList = List_Create();

  while ((c = *(str + strOffset)) != '\0') {
    switch (c) {
    case '(':
      Stack_PushChar(symbolStack, c);
      break;

    case ')':
      if (Stack_IsEmpty(symbolStack))
        Error();

      /* pop stack until '(' */
      while (*(char *)Stack_Top(symbolStack)->payload->data != '(')
        List_PushBack(outputList, Stack_Pop(symbolStack));
      /* pop '(' but not ouput */
      StackNode_Destroy(Stack_Pop(symbolStack));
      break;

    case '-':
    case '+':
      /* empty stack */
      if (!Stack_IsEmpty(symbolStack)) {
        /* pop stack until '+', '-', '(' */
        cInStack = *(char *)Stack_Top(symbolStack)->payload->data;
        while (cInStack != '+' && cInStack != '-' && cInStack != '(') {
          List_PushBack(outputList, Stack_Pop(symbolStack));
          if (!Stack_IsEmpty(symbolStack))
            break;

          cInStack = *(char *)Stack_Top(symbolStack)->payload->data;
        }

        if (!Stack_IsEmpty(symbolStack) &&
            *(char *)Stack_Top(symbolStack)->payload->data != '(') {
          List_PushBack(outputList, Stack_Pop(symbolStack));
        }
      }

      Stack_PushChar(symbolStack, c);
      List_PushBack_Char(outputList, ' ');
      break;

    case '/':
    case '*':
      /* empty stack */
      if (!Stack_IsEmpty(symbolStack)) {
        cInStack = *(char *)Stack_Top(symbolStack)->payload->data;
        /* pop stack until '+', '-', '(' */
        while (cInStack != '+' && cInStack != '-' && cInStack != '(') {
          List_PushBack(outputList, Stack_Pop(symbolStack));
          if (Stack_IsEmpty(symbolStack))
            break;

          cInStack = *(char *)Stack_Top(symbolStack)->payload;
        }
      }

      Stack_PushChar(symbolStack, c);
      List_PushBack_Char(outputList, ' ');
      break;

    default:
      /* operands, output immediately */
      if (c != ' ')
        List_PushBack_Char(outputList, c);
      break;
    }

    strOffset++;
  }

  /* output the left symbols in stack */
  while (!Stack_IsEmpty(symbolStack)) {
    List_PushBack(outputList, Stack_Pop(symbolStack));
  }

  /* print result */
  printf("postfix: ");
  node = outputList->first;
  while (node != NULL) {
    printf("%c", *(char *)node->payload->data);
    node = node->next;
  }
  printf("\n");

  /* clear */
  Stack_Destroy(symbolStack);
  List_Destroy(outputList);

  return NULL;
}
