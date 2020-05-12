#include "list.h"

/*
 * ************************************************************
 * list_node_t
 * ************************************************************
 */

list_node_t *ListNode_Create(void) {
  list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));
  memset(node, 0, sizeof(list_node_t));

  return node;
}

void ListNode_Destroy(list_node_t *node) {
  if (node->payload != NULL)
    Payload_Destroy(node->payload);

  free(node);
}

void ListNode_Print(list_node_t *node) {
  DBGLog("%p | ", (void *)node);
  if (node == NULL) {
    printf("null \n");
    return;
  }

  Payload_Print(node->payload);
}

/*
 * ************************************************************
 * list_t
 * ************************************************************
 */

list_t *List_Create(void) {
  list_t *ret = (list_t *)malloc(sizeof(list_t));
  memset(ret, 0, sizeof(list_t));

  ret->first = NULL;
  ret->last = NULL;

  return ret;
}

void List_Destroy(list_t *l) {
  list_node_t *prev = NULL;
  list_node_t *node = NULL;

  for (node = l->first; node != NULL;) {
    prev = node;
    node = node->next;
    ListNode_Destroy(prev);
  }

  free(l);
}

int List_IsEmpty(list_t *l) { return (int)(l->length == 0); }

list_node_t *List_Prev(list_t *l, list_node_t *node) {
  list_node_t **pprev = NULL;

  for (pprev = &(l->first); *pprev != NULL; pprev = &(*pprev)->next) {
    if ((*pprev)->next == node)
      break;
  }

  return *pprev;
}

list_node_t *List_Insert(list_t *l, list_node_t *where, list_node_t *node) {
  list_node_t *iter = l->first;

  /* inster front */
  if (where == l->first) {
    List_PushFront(l, node);
    return node;
  }

  /* find position node in list */
  while (iter->next != where && iter->next != NULL)
    iter = iter->next;

  /* position node is not in the list */
  if (iter->next != where)
    return node;

  /* insert(list, NULL, node) */
  if (iter->next == NULL)
    l->last = node;

  node->next = iter->next;
  iter->next = node;

  l->length++;

  return node;
}

list_node_t *List_PushBack(list_t *l, list_node_t *node) {
  list_node_t *last = l->last;

  l->length++;

  /* empty list */
  if (l->first == NULL) {
    l->first = node;
    l->last = node;
    node->next = NULL;
  } else {
    last->next = node;
    node->next = NULL;
    l->last = node;
  }

  return node;
}

list_node_t *List_PushFront(list_t *l, list_node_t *node) {
  /* empty list */
  if (l->last == NULL) {
    l->last = node;
    l->last->next = NULL;
  }

  node->next = l->first;
  l->first = node;
  l->length++;

  return node;
}

int List_Remove(list_t *l, list_node_t *node) {
  list_node_t *prev = NULL;
  list_node_t *cur = l->first;

  if (node == l->first) {
    l->first = l->first->next;
  } else {
    while (cur != NULL && cur != node && cur->next != NULL) {
      prev = cur;
      cur = cur->next;
    }

    /* can't find node in list */
    if (cur == NULL || cur != node)
      return 0;

    /* remove last node */
    if (cur == l->last)
      l->last = prev;

    prev->next = cur->next;
  }

  l->length--;

  ListNode_Destroy(cur);

  return 1;
}

list_node_t *List_Locate(list_t *l, int index) {
  int i = 0;
  list_node_t *ret = l->first;

  while (i != index) {
    ret = ret->next;
    i++;
  }

  return ret;
}

void List_Print(list_t *l) {
  list_node_t *node = l->first;

  DBGLog("**********************\n");
  DBGLog("List   : %p\n", (void *)l);
  DBGLog("length : %d\n", l->length);
  DBGLog("first  : %p\n", (void *)l->first);
  DBGLog("last   : %p\n", (void *)l->last);
  DBGLog("-------------\n");

  while (node != NULL) {
    ListNode_Print(node);

    node = node->next;
  }

  DBGLog("**********************\n");
}

/*
 * ************************************************************
 * helper
 * ************************************************************
 */
list_node_t *List_PushBack_Int(list_t *l, int data) {
  list_node_t *node = ListNode_Create();
  node->payload = Payload_CreateWithInt(data);
  List_PushBack(l, node);

  return node;
}

list_node_t *List_PushBack_Char(list_t *l, char data) {
  list_node_t *node = ListNode_Create();
  node->payload = Payload_CreateWithChar(data);
  List_PushBack(l, node);

  return node;
}

list_node_t *List_PushBack_String(list_t *l, const char *data) {
  list_node_t *node = ListNode_Create();
  node->payload = Payload_CreateWithString(data);
  List_PushBack(l, node);

  return node;
}

list_node_t *List_PushBack_UserDefine(list_t *l, size_t size, void *data) {
  list_node_t *node = ListNode_Create();
  node->payload = Payload_Create(PayloadType_UserDefine, size, data);
  List_PushBack(l, node);

  return node;
}

list_node_t *List_PushFront_Int(list_t *l, int data) {
  list_node_t *node = ListNode_Create();
  node->payload = Payload_CreateWithInt(data);
  List_PushFront(l, node);

  return node;
}

list_node_t *List_PushFront_Char(list_t *l, char data) {
  list_node_t *node = ListNode_Create();
  node->payload = Payload_CreateWithChar(data);
  List_PushFront(l, node);

  return node;
}

list_node_t *List_PushFront_String(list_t *l, const char *data) {
  list_node_t *node = ListNode_Create();
  node->payload = Payload_CreateWithString(data);
  List_PushFront(l, node);

  return node;
}

list_node_t *List_PushFront_UserDefine(list_t *l, size_t size, void *data) {
  list_node_t *node = ListNode_Create();
  node->payload = Payload_Create(PayloadType_UserDefine, size, data);
  List_PushFront(l, node);

  return node;
}

/*
 * ************************************************************
 * test
 * ************************************************************
 */
void List_Test(void) {
  int payload1 = 1;
  int payload2 = 2;
  char payload3 = 'k';
  char *payload4 = "payload 3";
  list_node_t *first = NULL;
  list_node_t *second = NULL;
  list_node_t *third = NULL;
  list_node_t *fourth = NULL;

  list_t *l = List_Create();

  first = ListNode_Create();
  first->payload = Payload_CreateWithInt(payload1);
  List_PushFront(l, first);

  second = ListNode_Create();
  second->payload = Payload_CreateWithInt(payload2);
  List_PushBack(l, second);

  third = ListNode_Create();
  third->payload = Payload_CreateWithChar(payload3);
  List_Insert(l, NULL, third);

  fourth = ListNode_Create();
  fourth->payload = Payload_CreateWithString(payload4);
  List_PushBack(l, fourth);

  List_Remove(l, first);

  List_Print(l);

  /* list_node_t *node1 = ListNode_Create(); */
  List_Destroy(l);
}
