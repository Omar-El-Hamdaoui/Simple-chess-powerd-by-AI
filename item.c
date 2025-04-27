#include "item.h"


Item* nodeAlloc() {
    Item* node = (Item*)malloc(sizeof(Item));
    if (node == NULL) {
        printf("Erreur allocation mémoire\n");
        exit(1);
    }
    node->depth = 0;
    node->parent = NULL;
    node->next = NULL;
    return node;
}
