#include "common.h"
#include "list.h"
#include "tree.h"

extern list_t *infix2posfix(const char *);

int main(int argc, char **argv) {
  infix2posfix("(123+234)*(35-47)/(55+63)");
  tree_test();
  memtrack_list_allocations();
  return 0;
}
