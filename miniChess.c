//
// Created by omar on 27/04/25.
//
#include <stdio.h>

#include "board.h"

int main() {
    Piece board[8][8];
    initBoard(board);
    printBoard(board);

    printf("\nDéplacer pion blanc e2 à e4...\n");
    movePawn(board, 6, 4, 4, 4); // e2 → e4
    printBoard(board);

    printf("\nDéplacer pion noir d7 à d5...\n");
    movePawn(board, 1, 3, 3, 3); // d7 → d5
    printBoard(board);

    return 0;
}
