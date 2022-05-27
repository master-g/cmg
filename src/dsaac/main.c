#include "common.h"
#include "list.h"
#include "tree.h"

extern list_t *Convert_Infix2Posfix(const char *);

int main(int argc, char* argv[]) {
  Convert_Infix2Posfix("(123+234)*(35-47)/(55+63)");
  Tree_Test();
  memtrack_list_allocations();
  return 0;
}