#ifndef EVALUATE_H
#define EVALUATE_H

#include "board.h"

int evaluate(Piece board[8][8], char player);
int evaluate_material(Piece board[8][8], char player);
int evaluate_center_control(Piece board[8][8], char player);
int evaluate_pawn_structure(Piece board[8][8], char player);
int has_pawn_on_file(Piece board[8][8], char player, int file);


#endif
