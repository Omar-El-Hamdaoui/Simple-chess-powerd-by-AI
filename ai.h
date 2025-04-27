#ifndef AI_H
#define AI_H

#include "board.h"

int minimax(Piece board[8][8], int depth, char player, int maximizingPlayer);
Item* chooseBestMove(Piece board[8][8], char player, int depth);


#endif
