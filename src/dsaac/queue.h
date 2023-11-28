#ifndef QUEUE_H_
#define QUEUE_H_

#include "list.h"

/*
 * ************************************************************
 * queue_node_t
 * ************************************************************
 */

typedef list_node_t queue_node_t;

#define queue_node_with list_node_with
#define queue_node_free list_node_free
#define queue_node_print list_node_print

/*
 * ************************************************************
 * queue_t
 * ************************************************************
 */

typedef list_t queue_t;

#define queue_alloc list_alloc
#define queue_free list_free
#define queue_is_empty list_is_empty
#define queue_len list_len
#define queue_enque list_push
#define queue_front(q) (q)->first
#define queue_rear(q) (q)->last

queue_node_t *queue_dequeue(queue_t *q);

void queue_print(queue_t *q);

/* test */
void queue_test(void);

#endif /* QUEUE_H_ */
