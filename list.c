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
 * @param data the data in a void * to add
 * @param size the size_t of the data to add
 */
void _list_add(node *head, void *data, size_t size)
{
  if (head->data == NULL)
  {
    head->data = malloc(size);
    memcpy(head->data, data, size);
    return;
  }

  node *to_add = malloc(sizeof(node));
  to_add->data = malloc(size);
  memcpy(to_add->data, data, size);
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
void list_add(linked_list *list, void *data, size_t size)
{
#ifdef DEBUG
  fprintf(stdout, "ADDING TO THE LIST\n");
#endif
  if (list->head == NULL)
  {
#ifdef DEBUG
    fprintf(stdout, "NULL HEAD\n");
#endif
    list->head = malloc(sizeof(node));
  }

  _list_add(list->head, data, size);
  list->size++;
}

/**
 * Get an item in the list at the index provided
 * 
 * @param head the head of the linked list
 * @param index the index to retreive from
 * 
 * @returns void * containing the data or NULL of the index is not valid
 */
void *_get(node *head, int index)
{
  if (head == NULL)
  {
    return NULL;
  }

  int i = 0;
  node *cur_node = head;

  while (cur_node != NULL)
  {
    if (i == index)
    {
      return cur_node->data;
    }

    cur_node = cur_node->next;
    i++;
  }

  return NULL;
}

/**
 * Get an item in the list at the index provided
 * 
 * @param list the linked list to search
 * @param index the index to retreive from
 * 
 * @returns void * containing the data or NULL of the index is not valid
 */
void *get(linked_list *list, int index)
{
  return _get(list->head, index);
}

/**
 * Helper method to determine if the list contains a particular value
 * 
 * @param head the head of the linked list
 * @param data the void* data to check if it is in the linked list
 * @param compar* a custom comparator to be passed in
 * 
 * @returns 1 if the list contains the data, 0 if not
 */
int _contains(node *head, void *data, int (*compar)(const void *, const void *))
{
  if (head == NULL)
  {
    return -1;
  }

  node *cur_node = head;
  while (cur_node != NULL)
  {
    if ((*compar)(cur_node->data, data) == 0)
    {
      return 1;
    }

    cur_node = cur_node->next;
  }

  return -1;
}

/**
 * Determine if the list contains a particular value
 * 
 * @param head the head of the linked list
 * @param data the void* data to check if it is in the linked list
 * @param compar* a custom comparator to be passed in
 * 
 * @returns 1 if the list contains the data, 0 if not
 */
int contains(linked_list *list, void *data, int (*compar)(const void *, const void *))
{
  return _contains(list->head, data, (*compar));
}

/**
 * Helper method to get the index of particular data
 * 
 * @param head the head of the linked list
 * @param data the void* data to check the index of
 * @param compar* a custom comparator to be passed in
 * 
 * @returns the index of the data, or -1 if not in the list
 */
int _index_of(node *head, void *data, int (*compar)(const void *, const void *))
{
  if (head == NULL || data == NULL)
  {
    return -1;
  }

  int i = 0;
  node *cur_node = head;
  while (cur_node != NULL)
  {
    if ((*compar)(cur_node->data, data) == 0)
    {
      return i;
    }

    i++;
    cur_node = cur_node->next;
  }

  return -1;
}

/**
 * Get the index of particular data
 * 
 * @param list a pointer to the linked list
 * @param data the void* data to check the index of
 * @param compar* a custom comparator to be passed in
 * 
 * @returns the index of the data, or -1 if not in the list
 */
int index_of(linked_list *list, void *data, int (*compar)(const void *, const void *))
{
  return _index_of(list->head, data, (*compar));
}

/**
 * Helper method to delete items from the list
 * 
 * @param head the head of the list
 * @param data the void* data to remove
 * @param compar* a custom comparator
 */
int _list_delete(node *head, void *data, int (*compar)(const void *, const void *))
{
  printf("WE ARE HERE\n");
  if (head == NULL || data == NULL)
  {
    return -1;
  }

  node *cur_node = head;
  node *prev = head;
  while (cur_node != NULL)
  {
    if ((*compar)(cur_node->data, data) == 0)
    {
      if ((*compar)(cur_node->data, head->data) == 0) // head of list
      {
        head = cur_node->next;
        free(cur_node->data);
        cur_node->next = NULL;
        return 0;
      }
      else if (cur_node->next == NULL) // back of list
      {
        prev->next = NULL;
        cur_node->next = NULL;
        free(cur_node->data);
        return 0;
      }
      else
      {
        prev->next = cur_node->next;
        free(cur_node->data);
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
 * @param data the void* data to remove from the list
 * @param compar* a custom comparator to be passed in
 * 
 * @returns 0 if removed, -1 if not found in the list
 */
int list_delete(linked_list *list, void *data, int (*compar)(const void *, const void *))
{
  return _list_delete(list->head, data, (*compar));
}