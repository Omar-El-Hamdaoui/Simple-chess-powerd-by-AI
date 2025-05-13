#include "evaluate.h"
#include "board.h"
#include "piece.h"
#include <stdlib.h>

#define PAWN_VALUE     100
#define KNIGHT_VALUE   320
#define BISHOP_VALUE   330
#define ROOK_VALUE     500
#define QUEEN_VALUE    900
#define KING_VALUE    10000

#define DOUBLED_PAWN_PENALTY    20
#define ISOLATED_PAWN_PENALTY   25
#define PASSED_PAWN_BONUS       30
#define MOBILITY_WEIGHT         5
#define CENTER_CONTROL_BONUS    5

int getPieceValue(char type) {
    switch(type) {
        case 'P': case 'p': return PAWN_VALUE;
        case 'N': case 'n': return KNIGHT_VALUE;
        case 'B': case 'b': return BISHOP_VALUE;
        case 'R': case 'r': return ROOK_VALUE;
        case 'Q': case 'q': return QUEEN_VALUE;
        case 'K': case 'k': return KING_VALUE;
        default: return 0;
    }
}

int evaluate_material(Piece board[8][8], char player) {
    int score = 0;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            Piece p = board[i][j];
            if (p.type == ' ') continue;
            int value = getPieceValue(p.type);
            if (p.color == player)
                score += value;
            else
                score -= value;
        }
    }
    return score;
}

int evaluate_center_control(Piece board[8][8], char player) {
    int bonus = 0;
    int center[4][2] = {{3,3},{3,4},{4,3},{4,4}}; // d4, e4, d5, e5
    for(int i = 0; i < 4; i++) {
        int x = center[i][0];
        int y = center[i][1];
        if (board[x][y].color == player)
            bonus += CENTER_CONTROL_BONUS;
        else if (board[x][y].color != ' ')
            bonus -= CENTER_CONTROL_BONUS;
    }
    return bonus;
}

int evaluate_pawn_structure(Piece board[8][8], char player) {
    int penalty = 0;
    for (int f = 0; f < 8; f++) {
        int pawnCount = 0;
        int isolated = 1;
        for (int r = 0; r < 8; r++) {
            Piece p = board[r][f];
            if (p.type == 'P' && p.color == player) {
                pawnCount++;
                if (f > 0 && has_pawn_on_file(board, player, f - 1)) isolated = 0;
                if (f < 7 && has_pawn_on_file(board, player, f + 1)) isolated = 0;
            }
        }
        if (pawnCount > 1) penalty -= DOUBLED_PAWN_PENALTY;
        if (isolated && pawnCount > 0) penalty -= ISOLATED_PAWN_PENALTY;
    }
    return penalty;
}

int has_pawn_on_file(Piece board[8][8], char player, int file) {
    for (int r = 0; r < 8; r++) {
        if (board[r][file].type == 'P' && board[r][file].color == player) return 1;
    }
    return 0;
}

int evaluate(Piece board[8][8], char player) {
    int material = evaluate_material(board, player);
    int center = evaluate_center_control(board, player);
    int pawns = evaluate_pawn_structure(board, player);
    // autres composants comme mobilité, sécurité du roi, menaces peuvent être ajoutés ici

    return material + center + pawns;
}
