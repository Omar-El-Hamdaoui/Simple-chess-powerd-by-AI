//
// Created by omar on 27/04/25.
//
#include <stdio.h>
#include <stdlib.h>

#include "ai.h"
#include "board.h"

int main() {
    Piece board[8][8];
    initBoard(board);
    printBoard(board);

    char currentPlayer = 'w'; // Commencent par Blancs
    int maxMoves = 10;         // Limiter le nombre total de coups
    int depth = 2;             // Profondeur de Minimax (pas trop grande pour aller vite)

    for (int moveNumber = 1; moveNumber <= maxMoves; moveNumber++) {
        printf("\n=== Coup %d (%s) ===\n", moveNumber, (currentPlayer == 'w') ? "Blancs" : "Noirs");

        // IA choisit le meilleur coup
        Item* bestMove = chooseBestMove(board, currentPlayer, depth);

        if (bestMove == NULL) {
            printf("Aucun coup possible pour %s. Partie terminée.\n", (currentPlayer == 'w') ? "Blancs" : "Noirs");
            break;
        }

        // Appliquer le meilleur coup sur le board
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = bestMove->board[i][j];
            }
        }

        // Afficher le nouveau plateau
        printBoard(board);

        // Changer de joueur
        currentPlayer = (currentPlayer == 'w') ? 'b' : 'w';

        // Libérer la mémoire
        free(bestMove);
    }

    printf("\n=== Partie terminée ===\n");

    return 0;
}
