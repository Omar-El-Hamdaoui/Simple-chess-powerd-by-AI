#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "list.h"

/* Item Functions */

Item *nodeAlloc()
{
  Item *node;

  node = (Item *)malloc(sizeof(Item));
  assert(node);

  node->board = NULL;
  node->parent = NULL;
  node->prev = NULL;
  node->next = NULL;
  node->f = node->g = node->h = (double)0.0;

  return node;
}

void freeItem(Item *node)
{
  if (node && node->board)
    free(node->board);
  if (node)
    free(node);
}

void initList(list_t *list_p)
{
  // this next line was a comment
  if (list_p == NULL)
    list_p = malloc(sizeof(list_t));
  assert(list_p);

  list_p->numElements = 0;
  list_p->first = NULL;
  list_p->last = NULL;
}

int listCount(list_t *list)
{
  return list->numElements;
}

// return an item with corresponding board , or null
Item *onList(list_t *list, char *board)
{
  Item *my_item = list->first;

  for (int i = 0; i < listCount(list); ++i)
  {
    if (My_strcmp_board(my_item->board, board))
    {
      return my_item;
    }
  }
  return NULL;
}

// method to compare boards return 0 if false 1 if True
// On a board of dimensions MAX_BOARD X WH_BOARD
int My_strcmp_board(const char *b1, const char *b2)
{

  for (int i = 0; i < MAX_BOARD * WH_BOARD; ++i)
  {
    if (b1[i] != b2[i])
    {
      return 0;
    }
    return 1;
  }
}

// return and remove first item
Item *popFirst(list_t *list) //
{
  assert(list->numElements > 0);
  Item *first = list->first;
  delList(list, first);

  return first;
}

// return and remove last item
Item *popLast(list_t *list) //
{
  assert(list->numElements > 0);

  Item *last = list->last;
  delList(list, last);

  return last;
}

// remove a node from list
void delList(list_t *list, Item *node)
{
  assert(list->numElements > 0);
  --list->numElements;

  if (node->next != NULL)
  {
    node->next->prev = node->prev;
  }
  if (node->prev != NULL)
  {
    node->prev->next = node->next;
  }
}

// return and remove best item with minimal f value
Item *popBest(list_t *list) // and remove the best board from the list.
{
  assert(list->numElements > 0);
  Item *bestItem = list->first;
  Item *item = list->first;
  while (item->next != NULL)
  {
    if (item->f < bestItem->f)
    {
      bestItem = item;
    }
    item = item->next;
  }
  return bestItem;
}

// add item in top
void addFirst(list_t *list, Item *node) // add in head
{
  if (list->numElements > 0)
  {
    list->first->prev = node;
    list->first = node;
    ++list->numElements;
  }
  else
  {
    list->first = node;
    list->last = node;
    ++list->numElements;
  }
}

// add item in queue
void addLast(list_t *list, Item *node) // add in tail
{

  if (list->numElements > 0)
  {
    list->last->next = node;
    list->last = node;
    ++list->numElements;
  }

  else
  {
    list->first = node;
    list->last = node;
    ++list->numElements;
  }
}

void cleanupList(list_t *list)
{
  Item *item = list->first;
  Item *nextItem;
  while (item->next != NULL)
  {
    nextItem = item->next;
    freeItem(item);
  }
  list->numElements = 0;
}

void printList(list_t list)
{
  Item *item = list.first;
  while (item)
  {
    printf("%.2f [%s] - ", item->f, item->board);
    item = item->next;
  }
  printf(" (nb_items: %d)\n", list.numElements);
}

// TEST LIST

int main()
{
  Item *item, *node;
  char str[3];

  list_t openList;

  initList(&openList);

  for (int i = 0; i < 10; i++)
  {
    item = nodeAlloc();
    item->f = i;

    sprintf(str, "%2d", i);

    item->board = strdup(str);

    item->size = 3;

    addLast(&openList, item);
  }

  printList(openList);
  printf("\n");

  for (int i = 20; i < 25; i++)
  {
    item = nodeAlloc();
    item->f = i;
    item->size = 3;
    sprintf(str, "%2d", i);
    item->board = strdup(str);
    addFirst(&openList, item);
  }

  printList(openList);
  printf("\n");

  node = popFirst(&openList);

  if (node)
  {
    printf("first: %.2f\n", node->f);
  }
  printList(openList);

  printf("\n");

  node = popLast(&openList);
  if (node)
    printf("last: %.2f\n", node->f);
  printList(openList);
  printf("\n");

  node = popBest(&openList);
  printf("best node = %.2f\n", node->f);
  printList(openList);
  printf("\n");

  strcpy(str, "23");
  node = onList(&openList, str);
  if (node)
    printf("found %s: %.2f!\n", str, node->f);
  printList(openList);
  printf("\n");

  node = popFirst(&openList);

  item = nodeAlloc();
  item->f = 50;
  item->size = 3;
  sprintf(str, "50");
  item->board = strdup(str);
  addLast(&openList, item);

  printf("clean\n");

  cleanupList(&openList);

  printList(openList);
  printf("\n");

  return 0;
}
