#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "debug.h"

/**
 * This is a helper Linked List class that I made as part
 * of a data structures library that was meant to be portable,
 * generic, and easy to use. I know, generics in C sounds spooky,
 * and at times, it is, but it works as long as you put
 * a little more legwork in.
 * 
 * See {@link https://github.com/ntuckertriplet/libdatastructures/blob/master/linkedlist.c}
 */

/**
 * Helper method to add item to the list
 * 
 * @param head the head node of the linked list
 * @param p the process to add to the list
 */
void _list_add(node *head, process *p)
{
  if (head->p == NULL)
  {
    head->p = malloc(sizeof(process));
    memcpy(head->p, p, sizeof(process));
    return;
  }

  node *to_add = malloc(sizeof(node));
  to_add->p = malloc(sizeof(process));
  memcpy(to_add->p, p, sizeof(process));
  to_add->next = NULL;

  node *cur_node = head;
  while (cur_node->next != NULL)
  {
    cur_node = cur_node->next;
  }

  cur_node->next = to_add;
}

/**
 * Add item to the list
 * 
 * @param list the list to add to
 * @param data the data in a void * to add
 * @param size the size_t of the data to add
 */
void list_add(linked_list *list, process *p)
{
#ifdef DEBUG
  fprintf(stdout, "ADDING TO THE LIST\n");
#endif
  if (list->head == NULL)
  {
#ifdef DEBUG
    fprintf(stdout, "NULL HEAD\n");
    fprintf(stdout, "ADDING PID %d AND NAME %s\n", p->pid, p->name);
#endif
    list->head = malloc(sizeof(node));
    list->head->p = malloc(sizeof(process));
    memcpy(list->head->p, p, sizeof(process));
    list->size++;
#ifdef DEBUG
    fprintf(stdout, "ADDED PID %d AND NAME %s\n", list->head->p->pid, list->head->p->name);
#endif
    return;
  }

  _list_add(list->head, p);
  list->size++;
}

/**
 * Helper method to delete items from the list
 * 
 * @param head the head of the list
 * @param data the void* data to remove
 * @param compar* a custom comparator
 */
int _list_delete(node *head, process *p)
{
#ifdef DEBUG
  fprintf(stdout, "WE ARE HERE\n");
#endif
  if (head == NULL || p == NULL)
  {
    return -1;
  }

  node *cur_node = head;
  node *prev = head;
  while (cur_node != NULL)
  {
    if (cur_node->p->pid == p->pid)
    {
      if (head->p->pid == cur_node->p->pid) // head of list
      {
        if (head->next == NULL) // if the head is the only member of the list
        {
          free(head->p);
          return 0;
        }
        else
        {
          head = cur_node->next;
          free(cur_node->p);
          cur_node->next = NULL;
          return 0;
        }
      }
      else if (cur_node->next == NULL) // back of list
      {
        prev->next = NULL;
        cur_node->next = NULL;
        free(cur_node->p);
        return 0;
      }
      else
      {
        prev->next = cur_node->next;
        free(cur_node->p);
        cur_node->next = NULL;
        return 0;
      }
    }

    prev = cur_node;
    cur_node = cur_node->next;
  }

  return -1;
}

/**
 * Delete an item from the list
 * 
 * @param head the head of the linked list
 * @param p the process to delete
 * 
 * @returns 0 if removed, -1 if not found in the list
 */
int list_delete(linked_list *list, process *p)
{
  int del = _list_delete(list->head, p);
  if (del < 0)
    return -1;

  list->size--;
  return 0;
}