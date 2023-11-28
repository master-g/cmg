#include "common.h"
#include "list.h"
#include "stack.h"

void Error(void) {
  printf("Invalid expression !\n");
  exit(0);
}

list_t *infix2posfix(const char *str) {
  size_t str_offset = 0;
  const list_node_t *node = NULL;
  char c;
  char c_in_stack;
  stack_t *sym_stack = stack_alloc();
  list_t *out_list = list_alloc();

  while ((c = *(str + str_offset)) != '\0') {
    switch (c) {
    case '(':
      stack_push(sym_stack, stack_node_with(pdata_from_u8(c), 1));
      break;

    case ')':
      if (stack_is_empty(sym_stack))
        Error();

      /* pop stack until '(' */
      while (stack_top(sym_stack)->payload->data.u8 != '(')
        list_push(out_list, stack_pop(sym_stack));
      /* pop '(' but not ouput */
      stack_node_free(stack_pop(sym_stack));
      break;

    case '-':
    case '+':
      /* empty stack */
      if (!stack_is_empty(sym_stack)) {
        /* pop stack until '+', '-', '(' */
        c_in_stack = stack_top(sym_stack)->payload->data.u8;
        while (c_in_stack != '+' && c_in_stack != '-' && c_in_stack != '(') {
          list_push(out_list, stack_pop(sym_stack));
          if (!stack_is_empty(sym_stack))
            break;

          c_in_stack = stack_top(sym_stack)->payload->data.u8;
        }

        if (!stack_is_empty(sym_stack) &&
            stack_top(sym_stack)->payload->data.u8 != '(') {
          list_push(out_list, stack_pop(sym_stack));
        }
      }

      stack_push(sym_stack, stack_node_with(pdata_from_u8(c), 1));
      list_push(out_list, list_node_with(pdata_from_u8(' '), 1));
      break;

    case '/':
    case '*':
      /* empty stack */
      if (!stack_is_empty(sym_stack)) {
        c_in_stack = stack_top(sym_stack)->payload->data.u8;
        /* pop stack until '+', '-', '(' */
        while (c_in_stack != '+' && c_in_stack != '-' && c_in_stack != '(') {
          list_push(out_list, stack_pop(sym_stack));
          if (stack_is_empty(sym_stack))
            break;

          c_in_stack = stack_top(sym_stack)->payload->data.u8;
        }
      }

      stack_push(sym_stack, stack_node_with(pdata_from_u8(c), 1));
      list_push(out_list, list_node_with(pdata_from_u8(' '), 1));
      break;

    default:
      /* operands, output immediately */
      if (c != ' ')
        list_push(out_list, list_node_with(pdata_from_u8(c), 1));
      break;
    }

    str_offset++;
  }

  /* output the left symbols in stack */
  while (!stack_is_empty(sym_stack)) {
    list_push(out_list, stack_pop(sym_stack));
  }

  /* print result */
  printf("postfix: ");
  node = out_list->first;
  while (node != NULL) {
    printf("%c", node->payload->data.u8);
    node = node->next;
  }
  printf("\n");

  /* clear */
  stack_free(sym_stack);
  list_free(out_list);

  return NULL;
}
