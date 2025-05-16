#include <stdlib.h>
#include <limits.h>
#include <string.h>    // memcpy
#include "ai.h"

#include <stdio.h>

#include "movegen.h"
#include "evaluate.h"
#include "list.h"
#include "tt.h"
#include "zobrist.h"

void ai_init(void) {
    init_zobrist();
    tt_init();
}

static int cmp_desc(const void* a, const void* b) {
    const Item* A = *(Item* const*)a;
    const Item* B = *(Item* const*)b;
    int va = evaluate(A->board, A->player);
    int vb = evaluate(B->board, B->player);
    return vb - va;  // plus grand d'abord
}

static int cmp_asc(const void* a, const void* b) {
    const Item* A = *(Item* const*)a;
    const Item* B = *(Item* const*)b;
    int va = evaluate(A->board, A->player);
    int vb = evaluate(B->board, B->player);
    return va - vb;  // plus petit d'abord
}

int minimax_ab(Piece board[8][8], int depth, char player,
               int maximizingPlayer, int alpha, int beta) {
    if (depth == 0)
        return evaluate(board, player);

    Item* moves = generateMoves(board, player);
    if (!moves)  // pas de coups → mat ou pat
        return isInCheck(board, player)
             ? (maximizingPlayer ? INT_MIN : INT_MAX)  // mat
             : 0;                                      // pat

    int value = maximizingPlayer ? INT_MIN : INT_MAX;
    for (Item* m = moves; m; m = m->next) {
        // **NOUVEAU** : rejeter tout coup laissant roi en échec
        if (isInCheck(m->board, player))
            continue;

        int score = minimax_ab(
            m->board,
            depth - 1,
            (player=='w') ? 'b' : 'w',
            !maximizingPlayer,
            alpha, beta
        );
        if (maximizingPlayer) {
            value = (score > value) ? score : value;
            alpha = (alpha > value) ? alpha : value;
        } else {
            value = (score < value) ? score : value;
            beta  = (beta  < value) ? beta  : value;
        }
        if (alpha >= beta) break;
    }

    freeList(moves);
    return value;
}



Item* chooseBestMove(Piece board[8][8], char player, int depth) {
    Item* bestMove  = NULL;
    int   bestScore = (player=='w') ? INT_MIN : INT_MAX;

    // Récupère tous les coups légaux (générés sans laisser son roi en échec)
    Item* moves = generateMoves(board, player);
    if (!moves) return NULL;

    for (Item* m = moves; m; m = m->next) {
        // --- **NOUVEAU** : on rejette tout coup qui laisserait
        //                  le roi en échec
        if (isInCheck(m->board, player))
            continue;

        int score = minimax_ab(
            m->board,
            depth-1,
            (player=='w') ? 'b' : 'w',
            player=='b',
            INT_MIN,
            INT_MAX
        );

        if ((player=='w' && score > bestScore) ||
            (player=='b' && score < bestScore)) {
            bestScore = score;
            if (bestMove) free(bestMove);
            bestMove = nodeAlloc();
            memcpy(bestMove->board, m->board, sizeof(bestMove->board));
            }
    }

    freeList(moves);
    return bestMove;
}


