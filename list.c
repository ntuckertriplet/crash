#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"

/**
 * Helper method to add item to the list
 * 
 * @param list the list to add to
 * @param p the process to add to the list
 */
void list_add(linked_list *list, process *p)
{
  if (list->head == NULL)
  {
    list->head = malloc(sizeof(node));
    list->head->p = malloc(sizeof(process));
    list->head->p->name = strdup(p->name);
    list->head->p->pid = p->pid;
    list->size++;
    return;
  }

  node *to_add = malloc(sizeof(node));
  to_add->p = malloc(sizeof(process));
  to_add->p->name = strdup(p->name);
  to_add->p->pid = p->pid;
  to_add->next = NULL;

  node *cur_node = list->head;
  while (cur_node->next != NULL)
  {
    cur_node = cur_node->next;
  }

  cur_node->next = to_add;
  list->size++;
}

/**
 * Delete an item from the list
 * 
 * @param list the linked list to delete from
 * @param p the process to delete
 * 
 * @returns 0 if removed, -1 if not found in the list
 */
void list_delete(linked_list *list, process *p)
{
  if (list == NULL || p == NULL)
    return;

  if (list->head == NULL || list->head->p == NULL)
    return;

  node *cur_node = list->head;
  if (list->head->p->pid == p->pid) // if the head matches
  {
    if (list->size == 1)
    {
      free(cur_node->p->name);
      free(cur_node->p);
      free(list->head);
      list->head = NULL;
      list->size--;
      return;
    }
    else
    {
      free(cur_node->p->name);
      free(cur_node->p);
      list->head = list->head->next;
      cur_node->next = NULL;
      free(cur_node);
      list->size--;
      return;
    }
  }

  node *prev = cur_node;
  cur_node = cur_node->next;
  while (cur_node != NULL)
  {
    if (cur_node->p->pid == p->pid)
    {
      free(cur_node->p->name);
      free(cur_node->p);
      prev->next = cur_node->next;
      cur_node->next = NULL;
      free(cur_node);
      list->size--;
      return;
    }

    prev = cur_node;
    cur_node = cur_node->next;
  }

  return;
}