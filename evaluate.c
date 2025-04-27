#include "evaluate.h"

int evaluateBoard(Piece board[8][8]) {
    int score = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            switch (board[i][j].type) {
                case 'P': score += 1; break;
                case 'p': score -= 1; break;
                case 'N': case 'B': score += 3; break;
                case 'n': case 'b': score -= 3; break;
                case 'R': score += 5; break;
                case 'r': score -= 5; break;
                case 'Q': score += 9; break;
                case 'q': score -= 9; break;
                case 'K': score += 1000; break;
                case 'k': score -= 1000; break;
            }
        }
    }
    return score;
}
