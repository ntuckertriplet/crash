#ifndef LIST_H_
#define LIST_H_

#include "process.h"

typedef struct linked_list
{
  struct node *head;
  int size;
} linked_list;

typedef struct node
{
  process *p;
  struct node *next;
} node;

void list_add(linked_list *list, process *p);
int list_delete(linked_list *list, process *p);

#endif