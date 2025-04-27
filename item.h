#ifndef ITEM_H
#define ITEM_H

struct Item_s;

typedef struct Item {
  Piece board[8][8];   // Une copie du plateau pour cet état de jeu
  int depth;           // La profondeur dans l'arbre de recherche (peut être 0 pour l'instant)
  char player;         // Le joueur courant : 'w' (blanc) ou 'b' (noir)
  struct Item *parent; // Lien vers le parent (peut être NULL si non utilisé)
  struct Item *next;   // Lien vers l'élément suivant dans la liste chaînée
} Item;


#endif
