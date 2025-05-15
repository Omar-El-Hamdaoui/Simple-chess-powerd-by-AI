#ifndef AI_H
#define AI_H

#include "board.h"
#include "list.h"

// Minimax avec élagage alpha–bêta
int minimax_ab(Piece board[8][8],
               int depth,
               char player,
               int maximizingPlayer,
               int alpha,
               int beta);

// Choisit le meilleur coup pour `player` à la profondeur donnée
Item* chooseBestMove(Piece board[8][8], char player, int depth);

void ai_init(void);

#endif // AI_H
