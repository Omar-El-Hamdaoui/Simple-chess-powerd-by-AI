//
// Created by omar on 27/04/25.
//
#include <stdio.h>

#include "board.h"

int main() {
    Piece board[8][8];
    initBoard(board);
    printBoard(board);

    printf("\nDéplacement du pion blanc e2 vers e4...\n");

    // Déplacer Pion de (6, 4) -> (4, 4)
    movePiece(board, 6, 4, 4, 4);

    printBoard(board);

    return 0;
}
