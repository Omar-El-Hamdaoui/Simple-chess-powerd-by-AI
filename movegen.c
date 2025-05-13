#include "movegen.h"
#include "evaluate.h"
#include "item.h"
#include "list.h"
#include <stdlib.h>
#include <string.h> // pour memcpy

Item* generateMoves(Piece board[8][8], char player) {
    Item* moveList = NULL;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece piece = board[i][j];

            if (piece.type != ' ' && piece.color == player) {
                for (int i2 = 0; i2 < 8; i2++) {
                    for (int j2 = 0; j2 < 8; j2++) {
                        Piece tempBoard[8][8];
                        memcpy(tempBoard, board, sizeof(tempBoard));

                        int success = 0;

                        switch (piece.type) {
                            case 'P': case 'p':
                                success = movePawn(tempBoard, i, j, i2, j2); break;
                            case 'R': case 'r':
                                success = moveRook(tempBoard, i, j, i2, j2); break;
                            case 'B': case 'b':
                                success = moveBishop(tempBoard, i, j, i2, j2); break;
                            case 'Q': case 'q':
                                success = moveQueen(tempBoard, i, j, i2, j2); break;
                            case 'N': case 'n':
                                success = moveKnight(tempBoard, i, j, i2, j2); break;
                            case 'K': case 'k':
                                success = moveKing(tempBoard, i, j, i2, j2); break;
                        }

                        if (success) {
                            handlePromotion(tempBoard, i2, j2, player);
                        }

                        if (success && !isInCheck(tempBoard, player)) {
                            Item* child = nodeAlloc();
                            memcpy(child->board, tempBoard, sizeof(tempBoard));
                            child->player = (player == 'w') ? 'b' : 'w';
                            child->depth = 0;
                            child->parent = NULL;

                            if (player == 'w') {
                                child->whiteKingMoved = (i == 7 && j == 4) ? 1 : 0;
                                child->whiteKingsideRookMoved = (i == 7 && j == 7) ? 1 : 0;
                                child->whiteQueensideRookMoved = (i == 7 && j == 0) ? 1 : 0;
                                child->blackKingMoved = 0;
                                child->blackKingsideRookMoved = 0;
                                child->blackQueensideRookMoved = 0;
                            } else {
                                child->blackKingMoved = (i == 0 && j == 4) ? 1 : 0;
                                child->blackKingsideRookMoved = (i == 0 && j == 7) ? 1 : 0;
                                child->blackQueensideRookMoved = (i == 0 && j == 0) ? 1 : 0;
                                child->whiteKingMoved = 0;
                                child->whiteKingsideRookMoved = 0;
                                child->whiteQueensideRookMoved = 0;
                            }

                            child->next = moveList;
                            moveList = child;
                        }
                    }
                }
            }
        }
    }

    // Créer un Item temporaire représentant l’état actuel
    Item* currentItem = nodeAlloc();
    memcpy(currentItem->board, board, sizeof(Piece) * 64);
    currentItem->player = player;

    // copier les flags de roque depuis le dernier coup connu si tu peux (sinon laisse à 0 par défaut)
    currentItem->whiteKingMoved = 0;
    currentItem->blackKingMoved = 0;
    currentItem->whiteKingsideRookMoved = 0;
    currentItem->whiteQueensideRookMoved = 0;
    currentItem->blackKingsideRookMoved = 0;
    currentItem->blackQueensideRookMoved = 0;

    tryCastling(board, player, currentItem, &moveList);
    free(currentItem);


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

int tryCastling(Piece board[8][8], char player, Item* parent, Item** moveList) {
    int row = (player == 'w') ? 7 : 0;

    // Roque côté roi (kingside)
    if ((player == 'w' && !parent->whiteKingMoved && !parent->whiteKingsideRookMoved) ||
        (player == 'b' && !parent->blackKingMoved && !parent->blackKingsideRookMoved)) {

        if (board[row][5].type == ' ' && board[row][6].type == ' ' &&
            !isInCheck(board, player)) {

            Piece tempBoard[8][8];
            memcpy(tempBoard, board, sizeof(Piece) * 64);

            // Déplacer roi
            tempBoard[row][6] = tempBoard[row][4];
            tempBoard[row][4].type = ' ';
            tempBoard[row][4].color = ' ';

            // Déplacer tour
            tempBoard[row][5] = tempBoard[row][7];
            tempBoard[row][7].type = ' ';
            tempBoard[row][7].color = ' ';

            if (!isInCheck(tempBoard, player)) {
                // 👇 ICI : appel à la fonction externe
                add_castling_move(tempBoard, moveList, parent, player, 1); // kingside
            }
        }
    }

    // Roque côté dame (queenside)
    if ((player == 'w' && !parent->whiteKingMoved && !parent->whiteQueensideRookMoved) ||
        (player == 'b' && !parent->blackKingMoved && !parent->blackQueensideRookMoved)) {

        if (board[row][1].type == ' ' && board[row][2].type == ' ' && board[row][3].type == ' ' &&
            !isInCheck(board, player)) {

            Piece tempBoard[8][8];
            memcpy(tempBoard, board, sizeof(Piece) * 64);

            // Déplacer roi
            tempBoard[row][2] = tempBoard[row][4];
            tempBoard[row][4].type = ' ';
            tempBoard[row][4].color = ' ';

            // Déplacer tour
            tempBoard[row][3] = tempBoard[row][0];
            tempBoard[row][0].type = ' ';
            tempBoard[row][0].color = ' ';

            if (!isInCheck(tempBoard, player)) {
                // 👇 ICI : appel à la fonction externe
                add_castling_move(tempBoard, moveList, parent, player, 0); // queenside
            }
        }
    }

    return 0;
}

// Fonction externe pour ajouter un mouvement de roque
void add_castling_move(Piece temp[8][8], Item** list, Item* parent_state, char player, int kingside) {
    Item* child = nodeAlloc();
    memcpy(child->board, temp, sizeof(Piece) * 64);
    child->player = (player == 'w') ? 'b' : 'w';
    child->depth = 0;
    child->parent = parent_state;

    // Mémorisation des mouvements de roque
    if (player == 'w') {
        child->whiteKingMoved = 1;
        child->whiteKingsideRookMoved = kingside ? 1 : 0;
        child->whiteQueensideRookMoved = kingside ? 0 : 1;
    } else {
        child->blackKingMoved = 1;
        child->blackKingsideRookMoved = kingside ? 1 : 0;
        child->blackQueensideRookMoved = kingside ? 0 : 1;
    }

    child->next = *list;
    *list = child;
}



void handlePromotion(Piece board[8][8], int i2, int j2, char player) {
    if (player == 'w' && i2 == 0 && board[i2][j2].type == 'P') {
        board[i2][j2].type = 'Q';
    } else if (player == 'b' && i2 == 7 && board[i2][j2].type == 'p') {
        board[i2][j2].type = 'q';
    }
}

int isCheckmate(Piece board[8][8], char player) {
    if (!isInCheck(board, player)) return 0;
    Item* moves = generateMoves(board, player);
    if (moves == NULL) return 1;
    freeList(moves);
    return 0;
}

int isStalemate(Piece board[8][8], char player) {
    if (isInCheck(board, player)) return 0;
    Item* moves = generateMoves(board, player);
    if (moves == NULL) return 1;
    freeList(moves);
    return 0;
}

