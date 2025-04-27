#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "list.h" // Pour manipuler les listes chaînées

Item* generateMoves(Piece board[8][8], char player);

#endif
