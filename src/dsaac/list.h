#ifndef LIST_H_
#define LIST_H_

#include "common.h"
#include "payload.h"

#define List_First(l) (l)->first
#define List_Last(l) (l)->last
#define List_Length(l) (l)->length

/*
 * ************************************************************
 * list_node_t
 * ************************************************************
 */

typedef struct list_node_t {
  struct list_node_t *next;
  payload_t *payload;

} list_node_t;

list_node_t *ListNode_Create(void);
void ListNode_Destroy(list_node_t *node);

void ListNode_Print(list_node_t *node);

/*
 * ************************************************************
 * list_t
 * ************************************************************
 */

typedef struct list_t {
  int length;
  list_node_t *last;
  list_node_t *first;

} list_t;

list_t *List_Create(void);

void List_Destroy(list_t *l);

int List_IsEmpty(list_t *l);

list_node_t *List_Prev(list_t *l, list_node_t *node);

list_node_t *List_Insert(list_t *l, list_node_t *where, list_node_t *node);

list_node_t *List_PushBack(list_t *l, list_node_t *node);

list_node_t *List_PushFront(list_t *l, list_node_t *node);

int List_Remove(list_t *l, list_node_t *node);

list_node_t *List_Locate(list_t *l, int index);

void List_Print(list_t *l);

/* helper functions */
list_node_t *List_PushBack_Int(list_t *l, int data);
list_node_t *List_PushBack_Char(list_t *l, char data);
list_node_t *List_PushBack_String(list_t *l, const char *data);
list_node_t *List_PushBack_UserDefine(list_t *l, size_t size, void *data);

list_node_t *List_PushFront_Int(list_t *l, int data);
list_node_t *List_PushFront_Char(list_t *l, char data);
list_node_t *List_PushFront_String(list_t *l, const char *data);
list_node_t *List_PushFront_UserDefine(list_t *l, size_t size, void *data);

/* test */
void List_Test(void);

#endif /* LIST_H_ */
