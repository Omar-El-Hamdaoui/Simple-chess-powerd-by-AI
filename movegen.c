#include "movegen.h"
#include "evaluate.h"
#include "item.h"
#include "list.h"
#include <stdlib.h>
#include <string.h> // pour memcpy

Item* generateMoves(Piece board[8][8], char player) {
    Item* moveList = NULL; // Liste finale des coups possibles

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece piece = board[i][j];

            if (piece.type != ' ' && piece.color == player) {
                for (int i2 = 0; i2 < 8; i2++) {
                    for (int j2 = 0; j2 < 8; j2++) {
                        // Créer une copie locale du plateau
                        Piece tempBoard[8][8];
                        memcpy(tempBoard, board, sizeof(tempBoard));

                        int success = 0;

                        // Appliquer le mouvement sur la copie
                        switch (piece.type) {
                            case 'P': case 'p':
                                success = movePawn(tempBoard, i, j, i2, j2);
                                break;
                            case 'R': case 'r':
                                success = moveRook(tempBoard, i, j, i2, j2);
                                break;
                            case 'B': case 'b':
                                success = moveBishop(tempBoard, i, j, i2, j2);
                                break;
                            case 'Q': case 'q':
                                success = moveQueen(tempBoard, i, j, i2, j2);
                                break;
                            case 'N': case 'n':
                                success = moveKnight(tempBoard, i, j, i2, j2);
                                break;
                            case 'K': case 'k':
                                success = moveKing(tempBoard, i, j, i2, j2);
                                break;
                        }

                        if (success) {
                            // Créer un nouvel Item
                            Item* child = nodeAlloc();
                            memcpy(child->board, tempBoard, sizeof(tempBoard));

                            child->player = (player == 'w') ? 'b' : 'w';
                            child->depth = 0;
                            child->parent = NULL;

                            child->next = moveList;
                            moveList = child;
                        }
                    }
                }
            }
        }
    }

    return moveList;
}
int isInCheck(Piece board[8][8], char color) {
    int king_row = -1, king_col = -1;

    // Trouver la position du roi
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j].type == 'K' && board[i][j].color == color) {
                king_row = i;
                king_col = j;
                break;
            }
        }
    }

    if (king_row == -1 || king_col == -1) return 0; // roi non trouvé

    char enemy = (color == 'w') ? 'b' : 'w';

    // Vérifier si une pièce ennemie peut atteindre le roi
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j].color == enemy) {
                char t = board[i][j].type;
                if ((t == 'P' || t == 'p') && movePawn(board, i, j, king_row, king_col)) return 1;
                if ((t == 'R' || t == 'r') && moveRook(board, i, j, king_row, king_col)) return 1;
                if ((t == 'B' || t == 'b') && moveBishop(board, i, j, king_row, king_col)) return 1;
                if ((t == 'Q' || t == 'q') && moveQueen(board, i, j, king_row, king_col)) return 1;
                if ((t == 'N' || t == 'n') && moveKnight(board, i, j, king_row, king_col)) return 1;
                if ((t == 'K' || t == 'k') && moveKing(board, i, j, king_row, king_col)) return 1;
            }
        }
    }

    return 0;
}

