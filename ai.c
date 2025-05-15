#include <stdlib.h>
#include <limits.h>
#include <string.h>    // memcpy
#include "ai.h"

#include <stdio.h>

#include "movegen.h"
#include "evaluate.h"
#include "list.h"

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
    if (depth == 0) return evaluate(board, player);

    Item* moves = generateMoves(board, player);
    if (moves == NULL) return evaluate(board, player);

    // 1) Mettre les Item* dans un tableau
    int cnt = 0;
    for (Item* m = moves; m; m = m->next) cnt++;
    Item** arr = malloc(cnt * sizeof(Item*));
    int i = 0;
    for (Item* m = moves; m; m = m->next)
        arr[i++] = m;

    // 2) Trier selon maximizing/minimizing
    qsort(arr, cnt, sizeof(Item*),
          maximizingPlayer ? cmp_desc : cmp_asc);

    int value = maximizingPlayer ? INT_MIN : INT_MAX;
    for (i = 0; i < cnt; i++) {
        Item* m = arr[i];
        int score = minimax_ab(m->board, depth - 1,
            (player=='w')?'b':'w',
            !maximizingPlayer,
            alpha, beta);

        if (maximizingPlayer) {
            if (score > value) value = score;
            if (value > alpha) alpha = value;
        } else {
            if (score < value) value = score;
            if (value < beta) beta = value;
        }
        if (alpha >= beta) break;  // coupure
    }

    free(arr);
    freeList(moves);
    return value;
}

Item* chooseBestMove(Piece board[8][8], char player, int depth) {
    Item* bestMove = NULL;
    int bestScore = (player=='w') ? INT_MIN : INT_MAX;

    Item* moves = generateMoves(board, player);
    if (!moves) return NULL;

    // même logique de tri avant de choisir
    int cnt = 0;
    for (Item* m = moves; m; m = m->next) cnt++;
    Item** arr = malloc(cnt * sizeof(Item*));
    int i = 0;
    for (Item* m = moves; m; m = m->next)
        arr[i++] = m;
    qsort(arr, cnt, sizeof(Item*),
          (player=='w') ? cmp_desc : cmp_asc);

    for (i = 0; i < cnt; i++) {
        printf("chooseBestMove(depth=%d) : %d coups légaux générés pour %c\n",
       depth, cnt, player);
        Item* m = arr[i];
        int score = minimax_ab(m->board, depth - 1,
            (player=='w')?'b':'w',
            player=='b', INT_MIN, INT_MAX);
        if ((player=='w' && score > bestScore) ||
            (player=='b' && score < bestScore)) {
            bestScore = score;
            if (bestMove) free(bestMove);
            bestMove = nodeAlloc();
            memcpy(bestMove->board, m->board, sizeof(bestMove->board));
        }
    }

    free(arr);
    freeList(moves);
    return bestMove;
}
