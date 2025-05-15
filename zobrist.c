// zobrist.c
#include "zobrist.h"
#include <stdlib.h>
#include <time.h>

static uint64_t Z[2 /*color*/][6 /*type P,N,B,R,Q,K*/][64];
static uint64_t Z_player;  // blanc ou noir à jouer

// mappe pièce à index 0..5
static int piece_index(char type) {
    switch (type) {
        case 'P': case 'p': return 0;
        case 'N': case 'n': return 1;
        case 'B': case 'b': return 2;
        case 'R': case 'r': return 3;
        case 'Q': case 'q': return 4;
        case 'K': case 'k': return 5;
        default: return -1;
    }
}

void init_zobrist() {
    srand(time(NULL));
    for (int c = 0; c < 2; c++)
        for (int p = 0; p < 6; p++)
            for (int sq = 0; sq < 64; sq++)
                Z[c][p][sq] = (((uint64_t)rand() << 32) ^ ((uint64_t)rand() << 16) ^ rand());

    Z_player = (((uint64_t)rand() << 32) ^ ((uint64_t)rand() << 16) ^ rand());
}

uint64_t zobrist_hash(Piece board[8][8], char player) {
    uint64_t h = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece pc = board[i][j];
            if (pc.type != ' ') {
                int c = (pc.color == 'w') ? 0 : 1;
                int pi = piece_index(pc.type);
                int sq = i*8 + j;
                h ^= Z[c][pi][sq];
            }
        }
    }
    if (player == 'b') h ^= Z_player;
    return h;
}
