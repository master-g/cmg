/**
 * Created by MasterG on 2020/5/19.
 */

#include <stdio.h>

#include "texas_utils.h"

int main(int argc, char *argv[]) {
  int index = 0;
  texas_hand_t *list = NULL;
  texas_hand_t *iter;

  texas_utils_dump_list(&list);

  for (iter = list; iter != NULL; iter = iter->next) {
    printf(
        "v: %4d, cards: %s, hand: %s, index: %d\n", iter->value, iter->str_card,
        iter->str_desc, index);
    index++;
  }

  texas_utils_free_list(list);

  return 0;
}
