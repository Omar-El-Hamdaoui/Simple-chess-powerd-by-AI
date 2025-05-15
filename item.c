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

    // Initialiser les flags de roque à 0 (aucun mouvement encore)
    node->whiteKingMoved = 0;
    node->blackKingMoved = 0;
    node->whiteKingsideRookMoved = 0;
    node->whiteQueensideRookMoved = 0;
    node->blackKingsideRookMoved = 0;
    node->blackQueensideRookMoved = 0;

    return node;
}
