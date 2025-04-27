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
    // Vérifier que les positions sont dans les limites du plateau
    if (from_row < 0 || from_row >= 8 || from_col < 0 || from_col >= 8 ||
        to_row < 0 || to_row >= 8 || to_col < 0 || to_col >= 8) {
        return 0; // Déplacement invalide
        }

    // Vérifier qu'il y a une pièce à déplacer
    if (board[from_row][from_col].type == ' ') {
        return 0; // Pas de pièce à déplacer
    }

    // Pour l'instant, autoriser tout déplacement (on ajoutera les règles ensuite)

    // Déplacer la pièce
    board[to_row][to_col] = board[from_row][from_col];

    // Vider l'ancienne case
    board[from_row][from_col].type = ' ';
    board[from_row][from_col].color = ' ';

    return 1; // Déplacement réussi
}

int movePawn(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece pawn = board[from_row][from_col];

    if (pawn.type != 'P' && pawn.type != 'p') {
        return 0; // Ce n'est pas un pion
    }

    int direction = (pawn.color == 'w') ? -1 : 1; // blanc monte (-1), noir descend (+1)

    // Mouvement d'une case en avant
    if (from_col == to_col && to_row == from_row + direction && board[to_row][to_col].type == ' ') {
        board[to_row][to_col] = pawn;
        board[from_row][from_col].type = ' ';
        board[from_row][from_col].color = ' ';
        return 1;
    }

    // Mouvement initial de deux cases
    if (from_col == to_col &&
        ((pawn.color == 'w' && from_row == 6) || (pawn.color == 'b' && from_row == 1)) &&
        to_row == from_row + 2 * direction &&
        board[from_row + direction][from_col].type == ' ' &&
        board[to_row][to_col].type == ' ') {

        board[to_row][to_col] = pawn;
        board[from_row][from_col].type = ' ';
        board[from_row][from_col].color = ' ';
        return 1;
        }

    // Prise en diagonale
    if ((to_col == from_col + 1 || to_col == from_col - 1) &&
        to_row == from_row + direction &&
        board[to_row][to_col].type != ' ' &&
        board[to_row][to_col].color != pawn.color) {

        board[to_row][to_col] = pawn;
        board[from_row][from_col].type = ' ';
        board[from_row][from_col].color = ' ';
        return 1;
        }

    return 0; // Déplacement non valide
}
