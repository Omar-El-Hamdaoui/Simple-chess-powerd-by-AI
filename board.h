
#ifndef BOARD_H
#define BOARD_H

#include "list.h"
#define BOARD_SIZE 8

typedef struct {
    char type;  // 'P', 'R', 'N', 'B', 'Q', 'K' pour blanc ; minuscule pour noir
    char color; // 'w' (blanc) ou 'b' (noir)
} Piece;

void initBoard(Piece board[BOARD_SIZE][BOARD_SIZE]);
void printBoard(Piece board[BOARD_SIZE][BOARD_SIZE]);

int movePiece(Piece board[8][8], int from_row, int from_col, int to_row, int to_col);


#endif
