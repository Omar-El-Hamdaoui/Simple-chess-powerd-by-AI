#include "ai.h"
#include "evaluate.h"
#include "movegen.h"

int minimax(Piece board[8][8], int depth, char player, int maximizingPlayer) {
    if (depth == 0) {
        return evaluateBoard(board);
    }

    if (maximizingPlayer) {
        int maxEval = -100000;
        Item *moves = generateMoves(board, player);
        for (Item *move = moves; move != NULL; move = move->next) {
            int eval = minimax(move->board, depth - 1, (player == 'w') ? 'b' : 'w', 0);
            if (eval > maxEval) maxEval = eval;
        }
        return maxEval;
    } else {
        int minEval = +100000;
        Item *moves = generateMoves(board, player);
        for (Item *move = moves; move != NULL; move = move->next) {
            int eval = minimax(move->board, depth - 1, (player == 'w') ? 'b' : 'w', 1);
            if (eval < minEval) minEval = eval;
        }
        return minEval;
    }
}
