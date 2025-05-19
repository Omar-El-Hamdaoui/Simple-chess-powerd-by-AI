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

int minimax_ab(Piece board[8][8],
               int depth,
               char player,
               int maximizingPlayer,
               int alpha,
               int beta)
{
    const int orig_alpha = alpha;
    uint64_t key = zobrist_hash(board, player);

    // 1) PROBE TT
    TTEntry* e = tt_probe(key);
    if (e && e->depth >= depth) {
        if (e->flag == EXACT) {
            return e->value;
        } else if (e->flag == LOWERBOUND) {
            if (e->value > alpha) alpha = e->value;
        } else if (e->flag == UPPERBOUND) {
            if (e->value < beta) beta = e->value;
        }
        if (alpha >= beta) {
            return e->value;
        }
    }

    // 2) Feuille
    if (depth == 0) {
        return evaluate(board, 'w');
    }

    // 3) Génération des coups
    Item* moves = generateMoves(board, player);
    if (!moves) {
        int leaf = isInCheck(board, player)
                   ? (maximizingPlayer ? INT_MIN : INT_MAX)
                   : 0;
        return leaf;
    }

    int value = maximizingPlayer ? INT_MIN : INT_MAX;

    // (optionnel) Transformer la liste chaînée en tableau
    // pour pouvoir faire un qsort/move ordering

    for (Item* m = moves; m; m = m->next) {
        if (isInCheck(m->board, player))
            continue;

        int score = minimax_ab(
            m->board,
            depth - 1,
            (player == 'w') ? 'b' : 'w',
            !maximizingPlayer,
            alpha,
            beta
        );

        if (maximizingPlayer) {
            if (score > value) value = score;
            if (value > alpha) alpha = value;
        } else {
            if (score < value) value = score;
            if (value < beta)  beta  = value;
        }
        if (alpha >= beta)
            break;
    }

    freeList(moves);

    // 4) STORE TT
    TTFlag flag;
    if      (value <= orig_alpha) flag = UPPERBOUND;
    else if (value >= beta)       flag = LOWERBOUND;
    else                          flag = EXACT;

    tt_store(key, depth, flag, value);

    return value;
}




Item* chooseBestMove(Piece board[8][8], char player, int depth) {
    Item* bestMove  = NULL;
    int   bestScore = (player=='w') ? INT_MIN : INT_MAX;

    // Récupère tous les coups légaux (générés sans laisser son roi en échec)
    Item* moves = generateMoves(board, player);
    if (!moves) return NULL;

    for (Item* m = moves; m; m = m->next) {
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


