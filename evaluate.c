#include "evaluate.h"
#include "board.h"
#include "piece.h"
#include "list.h"
#include "item.h"
#include "movegen.h"
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
#define DEVELOPMENT_BONUS       10



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

int has_pawn_on_file(Piece board[8][8], char player, int file) {
    for (int r = 0; r < 8; r++) {
        if (board[r][file].type == 'P' && board[r][file].color == player) return 1;
    }
    return 0;
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

    // Bonus pour les pions centraux avancés
    for (int r = 0; r < 8; r++) {
        for (int f = 3; f <= 4; f++) { // colonnes d et e
            Piece p = board[r][f];
            if (p.type == 'P' && p.color == player) {
                penalty += (6 - r) * 2;
            } else if (p.type == 'p' && p.color == player) {
                penalty += (r - 1) * 2;
            }
        }
    }

    return penalty;
}

int evaluate_mobility(Piece board[8][8], char player) {
    int mobility = 0;
    Item* moves = generateMoves(board, player);
    for (Item* m = moves; m != NULL; m = m->next) mobility++;
    freeList(moves);
    return mobility * MOBILITY_WEIGHT;
}

int evaluate_development(Piece board[8][8], char player) {
    int score = 0;
    int start_row = (player == 'w') ? 7 : 0;
    for (int j = 0; j < 8; j++) {
        Piece p = board[start_row][j];
        if ((p.type == 'B' || p.type == 'N' || p.type == 'R') && p.color == player) {
            score -= DEVELOPMENT_BONUS;
        }
    }
    return score;
}

int evaluate_king_safety(Piece board[8][8], char player) {
    int score = 0;
    int king_row = -1, king_col = -1;

    // Trouver le roi du joueur
    for (int i = 0; i < 8 && king_row == -1; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j].type == ((player == 'w') ? 'K' : 'k')) {
                king_row = i;
                king_col = j;
                break;
            }
        }
    }

    // Bonus si le roi est sur une colonne sûre (g ou c) — typique du roque
    if (king_col == 6 || king_col == 2)
        score += 40;

    // Malus si le roi est encore au centre (colonne e)
    if (king_col == 4)
        score -= 30;

    // Bonus si protégé par des pions devant lui
    int forward = (player == 'w') ? -1 : 1;
    int r = king_row + forward;
    for (int c = king_col - 1; c <= king_col + 1; c++) {
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            if (board[r][c].type == 'P' && player == 'w') score += 5;
            if (board[r][c].type == 'p' && player == 'b') score += 5;
        }
    }

    return score;
}

int evaluate_threats(Piece board[8][8], char player) {
    int score = 0;
    Item* myMoves = generateMoves(board, player);
    Item* opponentMoves = generateMoves(board, (player == 'w') ? 'b' : 'w');

    // Si je menace une pièce ennemie
    for (Item* m = myMoves; m != NULL; m = m->next) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // Si la case contient ma pièce dans le coup généré, mais contenait une pièce ennemie avant
                if (m->board[i][j].type != ' ' && m->board[i][j].color == player &&
                    board[i][j].type != ' ' && board[i][j].color != player) {
                    int value = getPieceValue(board[i][j].type);
                    score += value / 10;  // pondération, tu peux ajuster (ex: /5 si tu veux plus agressif)
                    }
            }
        }
    }

    // Si l’adversaire menace mes pièces
    for (Item* m = opponentMoves; m != NULL; m = m->next) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (m->board[i][j].type != ' ' && m->board[i][j].color != player &&
                    board[i][j].type != ' ' && board[i][j].color == player) {
                    int value = getPieceValue(board[i][j].type);
                    score -= value / 10;
                    }
            }
        }
    }

    freeList(myMoves);
    freeList(opponentMoves);
    return score;
}

int evaluate(Piece board[8][8], char player) {
    int material = evaluate_material(board, player);
    int center = evaluate_center_control(board, player);
    int pawns = evaluate_pawn_structure(board, player);
    int mobility = evaluate_mobility(board, player);
    int development = evaluate_development(board, player);
    int kingSafety = evaluate_king_safety(board, player);
    int threats = evaluate_threats(board, player);

    return material + center + pawns + mobility + threats+ development + kingSafety;

}