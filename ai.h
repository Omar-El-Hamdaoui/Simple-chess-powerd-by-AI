#ifndef AI_H
#define AI_H

#include "board.h"
#include "item.h"


int minimax(Piece board[8][8], int depth, char player, int maximizingPlayer, int alpha, int beta);
Item* chooseBestMove(Piece board[8][8], char player, int depth);


#endif
