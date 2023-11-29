#include "common.h"

#include "test.h"

extern void infix2posfix(const char *);

int main(int argc, char **argv) {
  test_payload();
  test_list();
  test_stack();
  test_queue();
  test_tree();

  infix2posfix("(123+234)*(35-47)/(55+63)");
  memtrack_list_allocations();
  return 0;
}
