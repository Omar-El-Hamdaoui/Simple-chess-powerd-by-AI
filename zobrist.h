#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <stdint.h>
#include "board.h"

void     init_zobrist();
uint64_t zobrist_hash(Piece board[8][8], char player);

#endif