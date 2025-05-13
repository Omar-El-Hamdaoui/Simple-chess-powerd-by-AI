#include <stdlib.h>
#include <assert.h>
#include "list.h"

#include <string.h>
#include <stdio.h>


#include "item.h"

/**
 * Allocate and initialize a new Item node.
 */
Item *nodeAlloc() {
    Item *node = malloc(sizeof(Item));
    if (!node) {
        fprintf(stderr, "Erreur d'allocation mémoire pour Item\\n");
        exit(EXIT_FAILURE);
    }

    memset(node, 0, sizeof(Item)); // initialise tout proprement, y compris parent=NULL
    return node;
}


/**
 * Free a single Item node.
 */
void freeItem(Item *node) {
    free(node);
}

/**
 * Initialize a list_t structure.
 */
void initList(list_t *list_p) {
    assert(list_p);
    list_p->numElements = 0;
    list_p->name = NULL;
    list_p->first = NULL;
    list_p->last = NULL;
}

/**
 * Return number of elements in the list.
 */
int listCount(list_t *list) {
    assert(list);
    return list->numElements;
}

/**
 * Find an Item in the list whose board matches the given board.
 * Returns pointer to Item or NULL if not found.
 */
Item *onList(list_t *list,char *board) {
    assert(list && board);
    for (Item *it = list->first; it; it = it->next) {
        /* Assume Item has a board field that is a char array of size BOARD_SIZE*BOARD_SIZE */
        if (memcmp(it->board, board, BOARD_SIZE * BOARD_SIZE) == 0) {
            return it;
        }
    }
    return NULL;
}

/**
 * Remove a node from the list without freeing it.
 */
void delList(list_t *list, Item *node) {
    assert(list && node && list->numElements > 0);

    if (list->first == node) {
        list->first = node->next;
    } else {
        Item *prev = list->first;
        while (prev && prev->next != node) {
            prev = prev->next;
        }
        assert(prev);
        prev->next = node->next;
        if (list->last == node) {
            list->last = prev;
        }
    }
    node->next = NULL;
    list->numElements--;
}

/**
 * Return and remove the first item in the list.
 */
Item *popFirst(list_t *list) {
    assert(list && list->numElements > 0);
    Item *first = list->first;
    list->first = first->next;
    if (!list->first) {
        list->last = NULL;
    }
    first->next = NULL;
    list->numElements--;
    return first;
}

/**
 * Return and remove the last item in the list.
 */
Item *popLast(list_t *list) {
    assert(list && list->numElements > 0);
    if (list->first == list->last) {
        return popFirst(list);
    }
    Item *prev = list->first;
    while (prev->next != list->last) {
        prev = prev->next;
    }
    Item *last = list->last;
    prev->next = NULL;
    list->last = prev;
    list->numElements--;
    return last;
}

/**
 * Return and remove the best item. Currently same as popFirst.
 */
Item *popBest(list_t *list) {
    /* No cost field: default to FIFO */
    return popFirst(list);
}

/**
 * Add a node at the beginning of the list.
 */
void addFirst(list_t *list, Item *node) {
    assert(list && node);
    node->next = list->first;
    list->first = node;
    if (!list->last) {
        list->last = node;
    }
    list->numElements++;
}

/**
 * Add a node at the end of the list.
 */
void addLast(list_t *list, Item *node) {
    assert(list && node);
    node->next = NULL;
    if (list->last) {
        list->last->next = node;
    } else {
        list->first = node;
    }
    list->last = node;
    list->numElements++;
}

/**
 * Free all items in the list and reset it.
 */
void cleanupList(list_t *list) {
    assert(list);
    Item *it = list->first;
    while (it) {
        Item *next = it->next;
        freeItem(it);
        it = next;
    }
    list->first = list->last = NULL;
    list->numElements = 0;
}

/**
 * Free a chain of Item nodes starting from the given node.
 */
void freeList(Item *list) {
    Item *current = list;
    while (current) {
        Item *next = current->next;
        freeItem(current);
        current = next;
    }
}