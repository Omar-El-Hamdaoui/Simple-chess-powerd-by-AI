#include "ai.h"
#include "evaluate.h"
#include "movegen.h"
#include <limits.h> // pour INT_MIN et INT_MAX
#include <stddef.h>

int minimax(Piece board[8][8], int depth, char player, int maximizingPlayer, int alpha, int beta) {
    if (depth == 0) {
        return evaluate(board, player);
    }

    Item *moves = generateMoves(board, player);
    int bestEval;

    if (maximizingPlayer) {
        bestEval = -100000;
        for (Item *move = moves; move != NULL; move = move->next) {
            int eval = minimax(move->board, depth - 1, (player == 'w') ? 'b' : 'w', 0, alpha, beta);
            if (eval > bestEval) bestEval = eval;
            if (eval > alpha) alpha = eval;
            if (beta <= alpha) break;
        }
    } else {
        bestEval = 100000;
        for (Item *move = moves; move != NULL; move = move->next) {
            int eval = minimax(move->board, depth - 1, (player == 'w') ? 'b' : 'w', 1, alpha, beta);
            if (eval < bestEval) bestEval = eval;
            if (eval < beta) beta = eval;
            if (beta <= alpha) break;
        }
    }

    freeList(moves); // Libère les coups générés
    return bestEval;
}




Item* chooseBestMove(Piece board[8][8], char player, int depth) {
    Item* moves = generateMoves(board, player);
    Item* bestMove = NULL;
    int bestValue = (player == 'w') ? INT_MIN : INT_MAX;

    for (Item* move = moves; move != NULL; move = move->next) {
        int eval = minimax(move->board, depth - 1, (player == 'w') ? 'b' : 'w', 0, -100000, 100000);

        if (player == 'w') {
            if (eval > bestValue) {
                bestValue = eval;
                bestMove = move;
            }
        } else {
            if (eval < bestValue) {
                bestValue = eval;
                bestMove = move;
            }
        }
    }

    if (bestMove != NULL) {
        Item* chosen = nodeAlloc();
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                chosen->board[i][j] = bestMove->board[i][j];
            }
        }
        chosen->player = (player == 'w') ? 'b' : 'w';
        chosen->depth = 0;
        chosen->next = NULL;

        freeList(moves);

        return chosen;
    }

    // 🧹 Libérer même si aucun meilleur coup trouvé
    freeList(moves);

    return NULL;
}
