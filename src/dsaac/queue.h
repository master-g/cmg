#ifndef QUEUE_H_
#define QUEUE_H_

#include "list.h"

/*
 * ************************************************************
 * queue_node_t
 * ************************************************************
 */

typedef list_node_t queue_node_t;

#define QueueNode_Create ListNode_Create
#define QueueNode_Destroy(n) ListNode_Destroy(n)
#define QueueNode_Print(n) ListNode_Print(n)

/*
 * ************************************************************
 * queue_t
 * ************************************************************
 */

typedef list_t queue_t;

#define Queue_Create List_Create
#define Queue_Destroy(q) List_Destroy(q)
#define Queue_IsEmpty(q) List_IsEmpty(q)
#define Queue_Length(q) List_Length(q)
#define Queue_Enqueue(q, n) List_PushBack(q, n);
#define Queue_Front(q) (q)->first
#define Queue_Rear(q) (q)->last

#define Queue_EnqueueInt(q, d) List_PushBack_Int(q, d)
#define Queue_EnqueueChar(q, d) List_PushBack_Char(q, d)
#define Queue_EnqueueString(q, d) List_PushBack_String(q, d)

queue_node_t *Queue_Dequeue(queue_t *q);

void Queue_Print(queue_t *q);

/* test */
void Queue_Test(void);

#endif /* QUEUE_H_ */
