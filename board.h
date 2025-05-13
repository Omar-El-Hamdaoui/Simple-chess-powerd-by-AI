
#ifndef BOARD_H
#define BOARD_H


#include "piece.h"
#define BOARD_SIZE 8

void initBoard(Piece board[BOARD_SIZE][BOARD_SIZE]);
void printBoard(Piece board[BOARD_SIZE][BOARD_SIZE]);

int movePiece(Piece board[8][8], int from_row, int from_col, int to_row, int to_col);
int movePawn(Piece board[8][8], int from_row, int from_col, int to_row, int to_col);
int moveRook(Piece board[8][8], int from_row, int from_col, int to_row, int to_col);
int moveBishop(Piece board[8][8], int from_row, int from_col, int to_row, int to_col);
int moveQueen(Piece board[8][8], int from_row, int from_col, int to_row, int to_col);
int moveKnight(Piece board[8][8], int from_row, int from_col, int to_row, int to_col);
int moveKing(Piece board[8][8], int from_row, int from_col, int to_row, int to_col);





#endif
