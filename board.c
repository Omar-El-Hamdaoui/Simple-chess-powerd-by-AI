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

int moveRook(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece rook = board[from_row][from_col];

    if (rook.type != 'R' && rook.type != 'r') {
        return 0; // Pas une tour
    }

    // Déplacement uniquement horizontal ou vertical
    if (from_row != to_row && from_col != to_col) {
        return 0; // Pas ligne droite
    }

    // Vérification qu'il n'y a pas d'obstacles entre départ et arrivée
    if (from_row == to_row) {
        // Mouvement horizontal
        int step = (to_col > from_col) ? 1 : -1;
        for (int col = from_col + step; col != to_col; col += step) {
            if (board[from_row][col].type != ' ') {
                return 0; // Obstacle
            }
        }
    } else if (from_col == to_col) {
        // Mouvement vertical
        int step = (to_row > from_row) ? 1 : -1;
        for (int row = from_row + step; row != to_row; row += step) {
            if (board[row][from_col].type != ' ') {
                return 0; // Obstacle
            }
        }
    }

    // Vérifier si la destination est vide ou contient une pièce ennemie
    if (board[to_row][to_col].type == ' ' || board[to_row][to_col].color != rook.color) {
        board[to_row][to_col] = rook;
        board[from_row][from_col].type = ' ';
        board[from_row][from_col].color = ' ';
        return 1; // Déplacement réussi
    }

    return 0; // Destination occupée par une pièce alliée
}



int moveBishop(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece bishop = board[from_row][from_col];

    if (bishop.type != 'B' && bishop.type != 'b') {
        return 0; // Ce n'est pas un fou
    }

    // Vérifie déplacement en diagonale : |row difference| == |col difference|
    if (abs(to_row - from_row) != abs(to_col - from_col)) {
        return 0; // Pas une diagonale
    }

    int step_row = (to_row > from_row) ? 1 : -1;
    int step_col = (to_col > from_col) ? 1 : -1;

    int row = from_row + step_row;
    int col = from_col + step_col;

    while (row != to_row && col != to_col) {
        if (board[row][col].type != ' ') {
            return 0; // Obstacle trouvé
        }
        row += step_row;
        col += step_col;
    }

    // Case de destination : vide ou ennemie
    if (board[to_row][to_col].type == ' ' || board[to_row][to_col].color != bishop.color) {
        board[to_row][to_col] = bishop;
        board[from_row][from_col].type = ' ';
        board[from_row][from_col].color = ' ';
        return 1; // Déplacement réussi
    }

    return 0; // Sinon, pas de déplacement possible
}


int moveQueen(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece queen = board[from_row][from_col];

    if (queen.type != 'Q' && queen.type != 'q') {
        return 0; // Ce n'est pas une reine
    }

    // Si déplacement ligne droite ➔ utiliser même code que tour
    if (from_row == to_row || from_col == to_col) {
        return moveRook(board, from_row, from_col, to_row, to_col);
    }

    // Si déplacement en diagonale ➔ utiliser même code que fou
    if (abs(to_row - from_row) == abs(to_col - from_col)) {
        return moveBishop(board, from_row, from_col, to_row, to_col);
    }

    return 0; // Ni ligne droite ni diagonale
}

#include <stdbool.h>

int moveKnight(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece knight = board[from_row][from_col];

    if (knight.type != 'N' && knight.type != 'n') {
        return 0; // Ce n'est pas un cavalier
    }

    int row_diff = abs(to_row - from_row);
    int col_diff = abs(to_col - from_col);

    // Vérifie que le mouvement est en "L" : (2,1) ou (1,2)
    if (!((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2))) {
        return 0; // Pas un mouvement de cavalier
    }

    // Destination vide ou ennemie
    if (board[to_row][to_col].type == ' ' || board[to_row][to_col].color != knight.color) {
        board[to_row][to_col] = knight;
        board[from_row][from_col].type = ' ';
        board[from_row][from_col].color = ' ';
        return 1; // Déplacement réussi
    }

    return 0; // Case occupée par une pièce alliée
}
#include <stdbool.h>

int moveKing(Piece board[8][8], int from_row, int from_col, int to_row, int to_col) {
    Piece king = board[from_row][from_col];

    if (king.type != 'K' && king.type != 'k') {
        return 0; // Ce n'est pas un roi
    }

    int row_diff = abs(to_row - from_row);
    int col_diff = abs(to_col - from_col);

    // Le roi ne peut bouger que d'une case
    if (row_diff > 1 || col_diff > 1) {
        return 0;
    }

    // Destination vide ou ennemie
    if (board[to_row][to_col].type == ' ' || board[to_row][to_col].color != king.color) {
        board[to_row][to_col] = king;
        board[from_row][from_col].type = ' ';
        board[from_row][from_col].color = ' ';
        return 1; // Déplacement réussi
    }

    return 0; // Case occupée par une pièce alliée
}


Item *generateMoves(Piece board[8][8], char player) {
    Item *moveList = NULL; // liste de tous les coups possibles

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece piece = board[i][j];
            if (piece.type != ' ' && piece.color == player) {
                for (int i2 = 0; i2 < 8; i2++) {
                    for (int j2 = 0; j2 < 8; j2++) {
                        int success = 0;

                        // Tester selon le type de pièce
                        switch (piece.type) {
                            case 'P': case 'p':
                                success = movePawn(board, i, j, i2, j2);
                                break;
                            case 'R': case 'r':
                                success = moveRook(board, i, j, i2, j2);
                                break;
                            case 'B': case 'b':
                                success = moveBishop(board, i, j, i2, j2);
                                break;
                            case 'Q': case 'q':
                                success = moveQueen(board, i, j, i2, j2);
                                break;
                            case 'N': case 'n':
                                success = moveKnight(board, i, j, i2, j2);
                                break;
                            case 'K': case 'k':
                                success = moveKing(board, i, j, i2, j2);
                                break;
                        }

                        if (success) {
                            // Si déplacement valide : sauvegarder la copie du plateau
                            Item *child = nodeAlloc();
                            for (int x = 0; x < 8; x++)
                                for (int y = 0; y < 8; y++)
                                    child->board[x][y] = board[x][y];

                            child->depth = 0;
                            child->player = (player == 'w') ? 'b' : 'w'; // changer le tour
                            child->next = moveList;
                            moveList = child;

                            // Annuler le coup pour continuer à explorer
                            for (int x = 0; x < 8; x++)
                                for (int y = 0; y < 8; y++)
                                    board[x][y] = child->parent->board[x][y]; // Revenir en arrière
                        }
                    }
                }
            }
        }
    }
    return moveList;
}

