/*
 *  main.c
 *  DSAAC
 *
 *  Created by Master.G on 23-11-30.
 *  Copyright (c) 2023 Master.G. All rights reserved.
 */
#include "common.h"

#include "test.h"

extern void infix2posfix(const char *);

int main(int argc, char **argv) {
  // test_payload();
  // test_list();
  // test_list_iter();
  // test_stack();
  // test_queue();
  test_tree();

  const size_t ass = strlen("asshole");
  printf("%lu\n", ass);

  infix2posfix("(123+234)*(35-47)/(55+63)");
  memtrack_list_allocations();
  return 0;
}
