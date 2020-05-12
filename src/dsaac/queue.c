#include "queue.h"

queue_node_t *Queue_Dequeue(queue_t *q) {
  queue_node_t *node = q->first;
  q->first = q->first->next;
  q->length--;

  if (q->first == NULL)
    q->last = NULL;

  return node;
}

void Queue_Print(queue_t *q) {
  queue_node_t *node = q->first;

  DBGLog("**********************\n");
  DBGLog("Queue  : %p\n", (void *)q);
  DBGLog("length : %d\n", q->length);
  DBGLog("Front  : %p\n", (void *)q->first);
  DBGLog("Rear   : %p\n", (void *)q->last);
  DBGLog("-------------\n");

  while (node != NULL) {
    QueueNode_Print(node);

    node = node->next;
  }

  DBGLog("**********************\n");
}

void Queue_Test(void) {
  int payload1 = 1;
  char payload2 = 's';
  char *payload3 = "this is a queue";
  queue_t *q = NULL;
  queue_node_t *node1 = NULL;
  queue_node_t *node2 = NULL;
  queue_node_t *node3 = NULL;

  node1 = QueueNode_Create();
  node1->payload = Payload_CreateWithInt(payload1);
  node2 = QueueNode_Create();
  node2->payload = Payload_CreateWithChar(payload2);
  node3 = QueueNode_Create();
  node3->payload = Payload_CreateWithString(payload3);

  q = Queue_Create();

  Queue_Enqueue(q, node1);
  Queue_Enqueue(q, node2);
  Queue_Enqueue(q, node3);

  Queue_EnqueueChar(q, 'a');

  node1 = Queue_Dequeue(q);
  QueueNode_Print(node1);

  QueueNode_Destroy(node1);

  Queue_Print(q);

  Queue_Destroy(q);
}