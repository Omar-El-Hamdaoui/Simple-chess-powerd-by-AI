#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "ai.h"
#include "list.h"
#include "movegen.h"

int main() {
    Piece board[8][8];
    initBoard(board);         // Initialise le plateau de départ
    printBoard(board);        // Affiche le plateau initial

    char currentPlayer = 'w'; // Les blancs commencent
    int maxMoves = 10;        // Nombre de coups maximum à simuler
    int depth = 2;            // Profondeur de l'IA

    for (int moveNumber = 1; moveNumber <= maxMoves; moveNumber++) {
        printf("\n=== Coup %d (%s) ===\n", moveNumber, (currentPlayer == 'w') ? "Blancs" : "Noirs");

        Item* bestMove = chooseBestMove(board, currentPlayer, depth);

        if (bestMove == NULL) {
            printf("Aucun coup possible pour %s. Partie terminée.\n", (currentPlayer == 'w') ? "Blancs" : "Noirs");
            break;
        }

        // Appliquer le meilleur coup
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = bestMove->board[i][j];
            }
        }

        printBoard(board); // Afficher l’état après le coup

        // Détection d'échec et mat / pat
        if (isCheckmate(board, (currentPlayer == 'w') ? 'b' : 'w')) {
            printf("Échec et mat ! %s perd.\n", (currentPlayer == 'w') ? "Noirs" : "Blancs");
            break;
        }
        if (isStalemate(board, (currentPlayer == 'w') ? 'b' : 'w')) {
            printf("Pat ! Partie nulle.\n");
            break;
        }


        currentPlayer = (currentPlayer == 'w') ? 'b' : 'w'; // Changer de joueur

        free(bestMove); // Libérer l’Item alloué
    }

    printf("\n=== Partie terminée ===\n");
    return 0;
}
