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

