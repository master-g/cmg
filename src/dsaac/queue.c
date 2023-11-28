#include "queue.h"

queue_node_t *queue_dequeue(queue_t *q) {
  queue_node_t *node = q->first;
  q->first = q->first->next;
  q->length--;

  if (q->first == NULL)
    q->last = NULL;

  return node;
}

void queue_print(queue_t *q) {
  const queue_node_t *node = q->first;

  DBGLog("**********************\n");
  DBGLog("Queue  : %p\n", (void *)q);
  DBGLog("length : %d\n", q->length);
  DBGLog("Front  : %p\n", (void *)q->first);
  DBGLog("Rear   : %p\n", (void *)q->last);
  DBGLog("-------------\n");

  while (node != NULL) {
    queue_node_print(node);

    node = node->next;
  }

  DBGLog("**********************\n");
}

void queue_test(void) {
  queue_t *q = NULL;
  queue_node_t *node1 = NULL;
  queue_node_t *node2 = NULL;
  queue_node_t *node3 = NULL;

  node1 = queue_node_with(pdata_from_u32(1), 1);
  node2 = queue_node_with(pdata_from_u8('q'), 1);
  node3 = queue_node_with(pdata_from_string("this is a queue", 0), 1);

  q = queue_alloc();

  queue_enque(q, node1);
  queue_enque(q, node2);
  queue_enque(q, node3);

  node1 = queue_dequeue(q);
  queue_node_print(node1);

  queue_node_free(node1);

  queue_print(q);
  queue_free(q);
}
