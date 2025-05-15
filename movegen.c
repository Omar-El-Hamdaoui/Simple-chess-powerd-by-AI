#include "movegen.h"
#include "evaluate.h"
#include "item.h"
#include "list.h"
#include <stdlib.h>
#include <string.h> // pour memcpy
#include <stdbool.h>


Item* generateMoves(Piece board[8][8], char player) {
    Item* moveList = NULL;
    // Savoir si, avant tout coup, le roi est déjà en échec
    bool kingInCheck = isInCheck(board, player);

    // Pour chaque pièce du joueur...
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        Piece piece = board[i][j];
        if (piece.type == ' ' || piece.color != player)
          continue;

        // Essayer tous les déplacements possibles
        for (int i2 = 0; i2 < 8; i2++) {
          for (int j2 = 0; j2 < 8; j2++) {

            // ────────── Nouveau bloc ──────────
            Piece tmp[8][8];
            if ( tryMove(tmp, board, i, j, i2, j2) ) {
                handlePromotion(tmp, i2, j2, player);

                if ( !isInCheck(tmp, player) ) {
                    /* ---- création et insertion de l’Item ---- */
                    Item* child = nodeAlloc();
                    memcpy(child->board, tmp, sizeof(tmp));
                    child->player = (player=='w') ? 'b' : 'w';
                    child->depth  = 0;
                    child->parent = NULL;


                    child->next = moveList;
                    moveList    = child;
                }
            }


            // Promotion si besoin
            handlePromotion(tmp, i2, j2, player);

            // Filtrer : ne garder que les coups qui éliminent l’échec
            if (isInCheck(tmp, player))
              continue;

            // Ce coup est légal → on l’ajoute
            Item* child = nodeAlloc();
            memcpy(child->board, tmp, sizeof(tmp));
            child->player = (player == 'w') ? 'b' : 'w';
            child->depth  = 0;
            child->parent = NULL;

            // Copier les indicateurs de roque (à adapter si tu stockes ces flags ailleurs)
            if (player == 'w') {
              child->whiteKingMoved          = (i == 7 && j == 4);
              child->whiteKingsideRookMoved  = (i == 7 && j == 7);
              child->whiteQueensideRookMoved = (i == 7 && j == 0);
              child->blackKingMoved = child->blackKingsideRookMoved = child->blackQueensideRookMoved = 0;
            } else {
              child->blackKingMoved          = (i == 0 && j == 4);
              child->blackKingsideRookMoved  = (i == 0 && j == 7);
              child->blackQueensideRookMoved = (i == 0 && j == 0);
              child->whiteKingMoved = child->whiteKingsideRookMoved = child->whiteQueensideRookMoved = 0;
            }

            // Insérer en tête
            child->next = moveList;
            moveList   = child;
          }
        }
      }
    }

    // Roque : uniquement si on n’était pas déjà en échec
    if (!kingInCheck) {
      Item* currentItem = nodeAlloc();
      memcpy(currentItem->board, board, sizeof(Piece) * 64);
      currentItem->player = player;
      // initialiser les flags de roque à 0 ou reprendre ceux du parent si disponibles...
      currentItem->whiteKingMoved = currentItem->blackKingMoved =
      currentItem->whiteKingsideRookMoved = currentItem->whiteQueensideRookMoved =
      currentItem->blackKingsideRookMoved = currentItem->blackQueensideRookMoved = 0;

      tryCastling(board, player, currentItem, &moveList);
      free(currentItem);
    }

    return moveList;
}


int isInCheck(Piece board[8][8], char color) {
    int king_row = -1, king_col = -1;

    // Trouver le roi
    for (int i = 0; i < 8 && king_row == -1; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j].type == 'K' && board[i][j].color == color) {
                king_row = i;
                king_col = j;
                break;
            }
        }
    }

    if (king_row == -1) return 0; // pas de roi trouvé

    char enemy = (color == 'w') ? 'b' : 'w';

    // Vérifier si une pièce adverse peut capturer le roi
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j].color == enemy) {
                char t = board[i][j].type;

                if ((t == 'P' || t == 'p') && canPawnMove(board, i, j, king_row, king_col)) return 1;
                if ((t == 'R' || t == 'r') && canRookMove(board, i, j, king_row, king_col)) return 1;
                if ((t == 'B' || t == 'b') && canBishopMove(board, i, j, king_row, king_col)) return 1;
                if ((t == 'Q' || t == 'q') && canQueenMove(board, i, j, king_row, king_col)) return 1;
                if ((t == 'N' || t == 'n') && canKnightMove(board, i, j, king_row, king_col)) return 1;
                if ((t == 'K' || t == 'k') && canKingMove(board, i, j, king_row, king_col)) return 1;
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

int canPawnMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) {
    Piece p = board[from_i][from_j];
    if (p.type != 'P' && p.type != 'p') return 0;

    int dir = (p.color == 'w') ? -1 : 1;

    // Capture diagonale uniquement (c’est ce qui menace le roi)
    if ((to_j == from_j + 1 || to_j == from_j - 1) &&
        to_i == from_i + dir &&
        board[to_i][to_j].color != ' ' &&
        board[to_i][to_j].color != p.color) {
        return 1;
        }

    return 0;
}

int canRookMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) {
    if (from_i != to_i && from_j != to_j) return 0;
    int step_i = (to_i > from_i) ? 1 : (to_i < from_i ? -1 : 0);
    int step_j = (to_j > from_j) ? 1 : (to_j < from_j ? -1 : 0);

    int i = from_i + step_i;
    int j = from_j + step_j;
    while (i != to_i || j != to_j) {
        if (board[i][j].type != ' ') return 0;
        i += step_i;
        j += step_j;
    }
    return board[to_i][to_j].color != board[from_i][from_j].color;
}

int canBishopMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) {
    if (abs(to_i - from_i) != abs(to_j - from_j)) return 0;
    int step_i = (to_i > from_i) ? 1 : -1;
    int step_j = (to_j > from_j) ? 1 : -1;

    int i = from_i + step_i;
    int j = from_j + step_j;
    while (i != to_i && j != to_j) {
        if (board[i][j].type != ' ') return 0;
        i += step_i;
        j += step_j;
    }
    return board[to_i][to_j].color != board[from_i][from_j].color;
}

int canQueenMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) {
    return canRookMove(board, from_i, from_j, to_i, to_j) ||
           canBishopMove(board, from_i, from_j, to_i, to_j);
}

int canKnightMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) {
    int dx = abs(to_i - from_i);
    int dy = abs(to_j - from_j);
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) return 0;
    return board[to_i][to_j].color != board[from_i][from_j].color;
}

int canKingMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) {
    int dx = abs(to_i - from_i);
    int dy = abs(to_j - from_j);
    if (dx > 1 || dy > 1) return 0;
    return board[to_i][to_j].color != board[from_i][from_j].color;
}


int tryMove(Piece dst[8][8],
            Piece src[8][8],
            int i,int j,int i2,int j2)
{
    memcpy(dst, src, sizeof(Piece)*64);
    Piece p = src[i][j];
    int ok = 0;
    switch(p.type){
        case 'P': case 'p': ok = movePawn (dst,i,j,i2,j2); break;
        case 'R': case 'r': ok = moveRook (dst,i,j,i2,j2); break;
        case 'B': case 'b': ok = moveBishop(dst,i,j,i2,j2); break;
        case 'N': case 'n': ok = moveKnight(dst,i,j,i2,j2); break;
        case 'Q': case 'q': ok = moveQueen (dst,i,j,i2,j2); break;
        case 'K': case 'k': ok = moveKing  (dst,i,j,i2,j2); break;
    }
    return ok;
}

