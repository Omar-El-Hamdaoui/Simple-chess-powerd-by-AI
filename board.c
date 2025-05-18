#include <stdio.h>
// #include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "board.h"



void initBoard(Piece board[BOARD_SIZE][BOARD_SIZE]) {
    // Vider le plateau
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j].type = ' ';
            board[i][j].color = ' ';
        }
    }

    // Placer pièces noires
    char black_pieces[] = {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'};
    for (int j = 0; j < BOARD_SIZE; j++) {
        board[0][j].type = black_pieces[j];
        board[0][j].color = 'b';
        board[1][j].type = 'p';
        board[1][j].color = 'b';
    }

    // Placer pièces blanches
    char white_pieces[] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};
    for (int j = 0; j < BOARD_SIZE; j++) {
        board[7][j].type = white_pieces[j];
        board[7][j].color = 'w';
        board[6][j].type = 'P';
        board[6][j].color = 'w';
    }
}


void printBoard(Piece board[BOARD_SIZE][BOARD_SIZE]) {
    printf("\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j].type != ' ') {
                printf("%c ", board[i][j].type);
            } else {
                printf(". ");
            }
        }
        printf("\n");
    }
    printf("  a b c d e f g h\n");
}

int movePiece(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    if (from_row < 0 || from_row >= 8 || from_col < 0 || from_col >= 8 ||
        to_row < 0 || to_row >= 8 || to_col < 0 || to_col >= 8) {
        return 0;
        }

    Piece src = board[from_row][from_col];
    Piece dest = board[to_row][to_col];

    if (src.type == ' ') return 0;

    // Interdire de capturer sa propre pièce
    if (dest.type != ' ' && dest.color == src.color) return 0;

    // Faire le déplacement
    board[to_row][to_col] = src;
    board[from_row][from_col].type = ' ';
    board[from_row][from_col].color = ' ';
    return 1;
}


int movePawn(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) {
    Piece p = board[from_i][from_j];
    if (p.type != 'P' && p.type != 'p') return 0;

    int dir = (p.color == 'w') ? -1 : 1;
    int start = (p.color == 'w') ? 6 : 1;

    // Avancer d'une case
    if (to_j == from_j && to_i == from_i + dir && board[to_i][to_j].type == ' ') {
        return movePiece(board, from_i, from_j, to_i, to_j);
    }

    // Avancer de deux cases depuis la position de départ
    if (to_j == from_j && from_i == start &&
        to_i == from_i + 2 * dir &&
        board[from_i + dir][to_j].type == ' ' &&
        board[to_i][to_j].type == ' ') {
        return movePiece(board, from_i, from_j, to_i, to_j);
        }

    // Capture diagonale
    if ((to_j == from_j + 1 || to_j == from_j - 1) &&
        to_i == from_i + dir &&
        board[to_i][to_j].type != ' ' &&
        board[to_i][to_j].color != p.color) {
        return movePiece(board, from_i, from_j, to_i, to_j);
        }

    return 0;
}


int moveRook(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece rook = board[from_row][from_col];
    if (rook.type != 'R' && rook.type != 'r') return 0;

    if (from_row != to_row && from_col != to_col) return 0;

    if (from_row == to_row) {
        int step = (to_col > from_col) ? 1 : -1;
        for (int col = from_col + step; col != to_col; col += step) {
            if (board[from_row][col].type != ' ') return 0;
        }
    } else {
        int step = (to_row > from_row) ? 1 : -1;
        for (int row = from_row + step; row != to_row; row += step) {
            if (board[row][from_col].type != ' ') return 0;
        }
    }

    return movePiece(board, from_row, from_col, to_row, to_col);
}



int moveBishop(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece bishop = board[from_row][from_col];
    if (bishop.type != 'B' && bishop.type != 'b') return 0;

    if (abs(to_row - from_row) != abs(to_col - from_col)) return 0;

    int step_row = (to_row > from_row) ? 1 : -1;
    int step_col = (to_col > from_col) ? 1 : -1;

    int row = from_row + step_row;
    int col = from_col + step_col;

    while (row != to_row && col != to_col) {
        if (board[row][col].type != ' ') return 0;
        row += step_row;
        col += step_col;
    }

    return movePiece(board, from_row, from_col, to_row, to_col);
}



int moveQueen(Piece board[8][8],
              int from_row, int from_col,
              int to_row,   int to_col) {
    Piece queen = board[from_row][from_col];
    // Vérifier qu'il s'agit bien d'une reine
    if (queen.type != 'Q' && queen.type != 'q') return 0;

    int dr = to_row   - from_row;
    int dc = to_col   - from_col;

    // Cas ligne droite (tour) ou diagonale (fou)
    bool straight = (dr == 0 || dc == 0);
    bool diagonal = (abs(dr) == abs(dc));

    if (!straight && !diagonal) {
        // Ni droit ni diagonal
        return 0;
    }

    // Vérifier qu'il n'y a pas d'obstacle entre from et to
    int step_r = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);
    int step_c = (dc == 0) ? 0 : (dc > 0 ? 1 : -1);
    int r = from_row + step_r;
    int c = from_col + step_c;
    while (r != to_row || c != to_col) {
        if (board[r][c].type != ' ') {
            // Un obstacle bloque la reine
            return 0;
        }
        r += step_r;
        c += step_c;
    }

    // Enfin, déplacer la reine (gère aussi la prise)
    return movePiece(board, from_row, from_col, to_row, to_col);
}



int moveKnight(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece knight = board[from_row][from_col];
    if (knight.type != 'N' && knight.type != 'n') return 0;

    int row_diff = abs(to_row - from_row);
    int col_diff = abs(to_col - from_col);

    if (!((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2))) return 0;

    return movePiece(board, from_row, from_col, to_row, to_col);
}



int moveKing(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece king = board[from_row][from_col];
    if (king.type != 'K' && king.type != 'k') return 0;

    if (abs(to_row - from_row) > 1 || abs(to_col - from_col) > 1) return 0;

    return movePiece(board, from_row, from_col, to_row, to_col);
}



void fenToBoard(const char *fen, Piece board[8][8]) {
    int row = 0, col = 0;
    for (const char *p = fen; *p && row < 8; ++p) {
        if (*p == '/') {
            ++row;
            col = 0;
        } else if (isdigit((unsigned char)*p)) {
            int empty = *p - '0';
            for (int i = 0; i < empty; ++i)
                board[row][col++] = (Piece){ ' ', ' ' };
        } else {
            char type = *p;
            char color = isupper((unsigned char)type) ? 'w' : 'b';
            board[row][col++] = (Piece){ type, color };
        }
    }
    // Tout ce qui reste devient vide
    while (row < 8) {
        while (col < 8)
            board[row][col++] = (Piece){ ' ', ' ' };
        ++row; col = 0;
    }
}



