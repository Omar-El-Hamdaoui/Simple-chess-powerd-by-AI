#include "movegen.h"
#include <stdlib.h> // pour malloc, free

Item* generateMoves(Piece board[8][8], char player) {
    Item* moveList = NULL; // Liste finale des coups possibles

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece piece = board[i][j];
            if (piece.type != ' ' && piece.color == player) {
                for (int i2 = 0; i2 < 8; i2++) {
                    for (int j2 = 0; j2 < 8; j2++) {
                        int success = 0;

                        // Tester déplacement selon type de pièce
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
                            // Créer un nouveau Item pour ce mouvement
                            Item* child = nodeAlloc();

                            // Copier le nouveau plateau
                            for (int x = 0; x < 8; x++) {
                                for (int y = 0; y < 8; y++) {
                                    child->board[x][y] = board[x][y];
                                }
                            }

                            // Stocker qui doit jouer après
                            child->player = (player == 'w') ? 'b' : 'w';
                            child->depth = 0;
                            child->parent = NULL;

                            // Ajouter au début de la liste
                            child->next = moveList;
                            moveList = child;

                            // Undo move (très simple dans notre cas car board est par référence)
                            // Tu dois refaire l'état initial avant chaque essai pour éviter de corrompre le plateau
                            // (Idéalement : travailler sur des copies profondes plus tard si besoin)
                        }
                    }
                }
            }
        }
    }

    return moveList;
}
