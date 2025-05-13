#ifndef PIECE_H
#define PIECE_H

typedef struct {
    char type;   // Le type de pièce : 'P', 'R', 'N', 'B', 'Q', 'K' (blancs) ou 'p', 'r', 'n', 'b', 'q', 'k' (noirs)
    char color;  // 'w' pour blanc, 'b' pour noir, ou ' ' si vide
} Piece;

#endif