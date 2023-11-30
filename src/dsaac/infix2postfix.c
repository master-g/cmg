/*
 *  infix2postfix.c
 *  DSAAC
 *
 *  Created by Master.G on 23-11-30.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */
#include "common.h"
#include "list.h"
#include "stack.h"

void Error(void) {
  printf("Invalid expression !\n");
  exit(0);
}

void infix2posfix_printer(const pdata *data) { printf("%c", pdata_u8(data)); }

void infix2posfix(const char *str) {
  size_t str_offset = 0;
  char c;
  char c_in_stack;
  const pdata *stack_top;
  stack_t *sym_stack = stack_alloc();
  list_t *out_list = list_alloc();

  while ((c = *(str + str_offset)) != '\0') {
    switch (c) {
    case '(':
      stack_push(sym_stack, pdata_from_u8(c));
      break;

    case ')':
      if (stack_is_empty(sym_stack))
        Error();

      /* pop stack until '(' */
      while ((c_in_stack = pdata_u8(stack_peek(sym_stack))) != '(') {
        list_push(out_list, stack_pop(sym_stack));
      }
      /* pop '(' but not ouput */
      stack_top = stack_pop(sym_stack);
      pdata_free(stack_top);
      break;

    case '-':
    case '+':
      /* empty stack */
      if (!stack_is_empty(sym_stack)) {
        /* pop stack until '+', '-', '(' */
        c_in_stack = pdata_u8(stack_peek(sym_stack));
        while (c_in_stack != '+' && c_in_stack != '-' && c_in_stack != '(') {
          list_push(out_list, stack_pop(sym_stack));
          if (!stack_is_empty(sym_stack))
            break;

          c_in_stack = pdata_u8(stack_peek(sym_stack));
        }

        if (!stack_is_empty(sym_stack) &&
            pdata_u8(stack_peek(sym_stack)) != '(') {
          list_push(out_list, stack_pop(sym_stack));
        }
      }

      stack_push(sym_stack, pdata_from_u8(c));
      list_push(out_list, pdata_from_u8(' '));
      break;

    case '/':
    case '*':
      /* empty stack */
      if (!stack_is_empty(sym_stack)) {
        c_in_stack = pdata_u8(stack_peek(sym_stack));
        /* pop stack until '+', '-', '(' */
        while (c_in_stack != '+' && c_in_stack != '-' && c_in_stack != '(') {
          list_push(out_list, stack_pop(sym_stack));
          if (stack_is_empty(sym_stack))
            break;

          c_in_stack = pdata_u8(stack_peek(sym_stack));
        }
      }

      stack_push(sym_stack, pdata_from_u8(c));
      list_push(out_list, pdata_from_u8(' '));
      break;

    default:
      /* operands, output immediately */
      if (c != ' ')
        list_push(out_list, pdata_from_u8(c));
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
  list_foreach(out_list, infix2posfix_printer);

  /* clear */
  while (!stack_is_empty(sym_stack)) {
    const pdata *data = stack_pop(sym_stack);
    pdata_free(data);
  }
  stack_free(sym_stack);

  list_foreach(out_list, pdata_free);
  list_free(out_list);
}
