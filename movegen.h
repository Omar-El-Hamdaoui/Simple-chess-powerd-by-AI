#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "list.h"

// Fonction pour générer tous les coups possibles pour un joueur donné
Item* generateMoves(Piece board[8][8], char player);
int isInCheck(Piece board[8][8], char player);
int tryCastling(Piece board[8][8], char player, Item** moveList);
void handlePromotion(Piece board[8][8], int i2, int j2, char player);
int isCheckmate(Piece board[8][8], char player);
int isStalemate(Piece board[8][8], char player);



#endif
