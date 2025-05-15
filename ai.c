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
    Item* moves     = generateMoves(board, player);
    Item* best      = NULL;
    int maximizing  = (player == 'w');
    int bestScore   = maximizing ? INT_MIN : INT_MAX;

    for (Item* m = moves; m != NULL; m = m->next) {
        // Appel à la version alpha-beta de minimax
        int score = minimax(
            m->board,
            depth - 1,
            (player == 'w') ? 'b' : 'w',
            !maximizing,
            INT_MIN,
            INT_MAX
        );

        if ((maximizing && score > bestScore) ||
            (!maximizing && score < bestScore)) {
            bestScore = score;
            best      = m;
            }
    }

    // On retourne l'Item choisi (ou NULL si aucun coup)
    return best;
}

