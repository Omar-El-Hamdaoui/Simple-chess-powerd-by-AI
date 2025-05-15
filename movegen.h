#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "list.h"

// Fonction pour générer tous les coups possibles pour un joueur donné
Item* generateMoves(Piece board[8][8], char player);
int isInCheck(Piece board[8][8], char player);
int tryCastling(Piece board[8][8], char player, Item* parent, Item** moveList);
void handlePromotion(Piece board[8][8], int i2, int j2, char player);
int isCheckmate(Piece board[8][8], char player);
int isStalemate(Piece board[8][8], char player);
void add_castling_move(Piece temp[8][8], Item** list, Item* parent_state, char player, int kingside);

int canPawnMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j);

int canRookMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) ;
int canBishopMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) ;
int canQueenMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j) ;
int canKnightMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j);
int canKingMove(Piece board[8][8], int from_i, int from_j, int to_i, int to_j);

int tryMove(Piece dst[8][8],Piece src[8][8],int i,int j,int i2,int j2);



#endif
