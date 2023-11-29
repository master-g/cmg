#ifndef STACK_H_
#define STACK_H_

#include "list.h"

#define stack_t list_t

#define stack_alloc list_alloc
#define stack_free list_free
#define stack_is_empty list_is_empty
#define stack_len list_len

#define stack_peek list_last
#define stack_push list_push
#define stack_pop list_remove_last

#endif /* STACK_H_ */
