#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "movegen.h"

#include <ctype.h>
#include <stdio.h>

#include "item.h"
#include "list.h"
#include "evaluate.h"


// vecteurs de déplacement
static const int knight_dirs[8][2] = {
    {+2,+1},{+2,-1},{-2,+1},{-2,-1},
    {+1,+2},{+1,-2},{-1,+2},{-1,-2}
};
static const int king_dirs[8][2] = {
    {+1,0},{-1,0},{0,+1},{0,-1},
    {+1,+1},{+1,-1},{-1,+1},{-1,-1}
};

static void add_move(Item** list, Piece tmp[8][8], char playerWhoMoved) {
    // Si, après ce coup, **son** roi est en échec, on rejette
    if (isInCheck(tmp, playerWhoMoved))
        return;

    Item* it = nodeAlloc();
    memcpy(it->board, tmp, sizeof it->board);
    // On stocke qui doit jouer ensuite
    it->player = (playerWhoMoved == 'w') ? 'b' : 'w';
    it->next   = *list;
    *list      = it;
}





Item* generateMoves(Piece board[8][8], char player) {
    // Normalize player to lowercase 'w' or 'b'
    char me    = tolower((unsigned char)player);
    char enemy = (me == 'w') ? 'b' : 'w';
    Item* moves = NULL;
    Piece tmp[8][8];

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece p = board[i][j];
            // skip empty squares and opponent pieces
            if (p.type == ' ' || tolower((unsigned char)p.color) != me)
                continue;

            switch (toupper((unsigned char)p.type)) {
                case 'P': {
                    int dir   = (me == 'w') ? -1 : +1;
                    int start = (me == 'w') ? 6  : 1;
                    // one-square advance
                    if (i+dir>=0 && i+dir<8 && board[i+dir][j].type == ' ') {
                        memcpy(tmp, board, sizeof tmp);
                        tmp[i+dir][j] = p;
                        tmp[i][j].type = tmp[i][j].color = ' ';
                        handlePromotion(tmp, i+dir, j, me);
                        add_move(&moves, tmp, me);
                        // two-square initial advance
                        if (i == start && board[i+2*dir][j].type == ' ') {
                            memcpy(tmp, board, sizeof tmp);
                            tmp[i+2*dir][j] = p;
                            tmp[i][j].type = tmp[i][j].color = ' ';
                            add_move(&moves, tmp, me);
                        }
                    }
                    // diagonal captures
                    for (int dj = -1; dj <= 1; dj += 2) {
                        int ni = i+dir, nj = j+dj;
                        if (ni>=0 && ni<8 && nj>=0 && nj<8
                            && tolower((unsigned char)board[ni][nj].color) == enemy)
                        {
                            memcpy(tmp, board, sizeof tmp);
                            tmp[ni][nj] = p;
                            tmp[i][j].type = tmp[i][j].color = ' ';
                            handlePromotion(tmp, ni, nj, me);
                            add_move(&moves, tmp, me);
                        }
                    }
                    break;
                }

                case 'N':
                    for (int d = 0; d < 8; ++d) {
                        int ni = i + knight_dirs[d][0];
                        int nj = j + knight_dirs[d][1];
                        if (ni<0||ni>7||nj<0||nj>7) continue;
                        if (tolower((unsigned char)board[ni][nj].color) == me) continue;
                        memcpy(tmp, board, sizeof tmp);
                        tmp[ni][nj] = p;
                        tmp[i][j].type = tmp[i][j].color = ' ';
                        add_move(&moves, tmp, me);
                    }
                    break;

                case 'B': case 'R': case 'Q': {
                    int startd = (toupper((unsigned char)p.type)=='B') ? 4 : 0;
                    int endd   = (toupper((unsigned char)p.type)=='R') ? 4 : 8;
                    if (toupper((unsigned char)p.type) == 'Q') { startd = 0; endd = 8; }
                    for (int d = startd; d < endd; ++d) {
                        int di = king_dirs[d][0], dj = king_dirs[d][1];
                        for (int s = 1; ; ++s) {
                            int ni = i + di*s, nj = j + dj*s;
                            if (ni<0||ni>7||nj<0||nj>7) break;
                            char c = tolower((unsigned char)board[ni][nj].color);
                            if (c == me) break;
                            memcpy(tmp, board, sizeof tmp);
                            tmp[ni][nj] = p;
                            tmp[i][j].type = tmp[i][j].color = ' ';
                            add_move(&moves, tmp, me);
                            if (c == enemy) break;
                        }
                    }
                    break;
                }

                case 'K':
                    // king moves
                    for (int d = 0; d < 8; ++d) {
                        int ni = i + king_dirs[d][0];
                        int nj = j + king_dirs[d][1];
                        if (ni<0||ni>7||nj<0||nj>7) continue;
                        if (tolower((unsigned char)board[ni][nj].color) == me) continue;
                        memcpy(tmp, board, sizeof tmp);
                        tmp[ni][nj] = p;
                        tmp[i][j].type = tmp[i][j].color = ' ';
                        add_move(&moves, tmp, me);
                    }
                    // castling
                    {
                        Item* scratch = nodeAlloc();
                        memcpy(scratch->board, board, sizeof scratch->board);
                        scratch->player = me;
                        scratch->whiteKingMoved = scratch->whiteKingsideRookMoved =
                        scratch->whiteQueensideRookMoved = scratch->blackKingMoved =
                        scratch->blackKingsideRookMoved = scratch->blackQueensideRookMoved = 0;
                        tryCastling(board, me, scratch, &moves);
                        free(scratch);
                    }
                    break;

                default:
                    break;
            }
        }
    }

    return moves;
}


int isInCheck(Piece board[8][8], char color) {
    int king_row = -1, king_col = -1;

    // Trouver le roi
    for (int i = 0; i < 8 && king_row == -1; i++) {
        for (int j = 0; j < 8; j++) {
            if ((board[i][j].type == 'K' || board[i][j].type == 'k')
        && board[i][j].color == color) {
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


int tryCastling(Piece board[8][8],
                char player,
                Item* parent,
                Item** moveList)
{
    int row = (player=='w') ? 7 : 0;
    char kingType = (player=='w') ? 'K' : 'k';
    char rookType = (player=='w') ? 'R' : 'r';
    char enemy    = (player=='w') ? 'b' : 'w';

    // 0) Le roi doit être sur sa case d'origine
    if (board[row][4].type != kingType) return 0;

    // Flags parent
    bool kingMoved = (player=='w') ? parent->whiteKingMoved
                                  : parent->blackKingMoved;
    bool rookKSMoved = (player=='w') ? parent->whiteKingsideRookMoved
                                     : parent->blackKingsideRookMoved;
    bool rookQSMoved = (player=='w') ? parent->whiteQueensideRookMoved
                                     : parent->blackQueensideRookMoved;

    // ========== Roque côté roi ==========
    // Tour en h1/h8, cases f,g libres, ni le roi ni la tour n'ont bougé
    if (!kingMoved && !rookKSMoved
        && board[row][7].type == rookType
        && board[row][5].type == ' '
        && board[row][6].type == ' ')
    {
        // 1) Le roi ne doit pas être en échec sur e-file, f-file, g-file
        //    (on teste en simulant le roi sur chaque case)
        bool passesThroughCheck = false;
        for (int s = 4; s <= 6; ++s) {
            Piece tmp1[8][8];
            memcpy(tmp1, board, sizeof tmp1);
            // on déplace temporairement le roi
            tmp1[row][s] = tmp1[row][4];
            tmp1[row][4].type = tmp1[row][4].color = ' ';
            if (isInCheck(tmp1, player)) {
                passesThroughCheck = true;
                break;
            }
        }
        if (!passesThroughCheck) {
            // OK, on crée le coup final
            Piece tmp2[8][8];
            memcpy(tmp2, board, sizeof tmp2);
            // déplace roi et tour
            tmp2[row][6] = tmp2[row][4];
            tmp2[row][4].type = tmp2[row][4].color = ' ';
            tmp2[row][5] = tmp2[row][7];
            tmp2[row][7].type = tmp2[row][7].color = ' ';

            // on propague les flags : roi et tour ont bougé
            add_castling_move(tmp2, moveList, parent, player, /*kingside=*/1);
        }
    }

    // ========== Roque côté dame ==========
    // Tour en a1/a8, cases b,c,d libres, ni le roi ni la tour n'ont bougé
    if (!kingMoved && !rookQSMoved
        && board[row][0].type == rookType
        && board[row][1].type == ' '
        && board[row][2].type == ' '
        && board[row][3].type == ' ')
    {
        bool passesThroughCheck = false;
        for (int s = 4; s >= 2; --s) {
            Piece tmp1[8][8];
            memcpy(tmp1, board, sizeof tmp1);
            tmp1[row][s] = tmp1[row][4];
            tmp1[row][4].type = tmp1[row][4].color = ' ';
            if (isInCheck(tmp1, player)) {
                passesThroughCheck = true;
                break;
            }
        }
        if (!passesThroughCheck) {
            Piece tmp2[8][8];
            memcpy(tmp2, board, sizeof tmp2);
            tmp2[row][2] = tmp2[row][4];
            tmp2[row][4].type = tmp2[row][4].color = ' ';
            tmp2[row][3] = tmp2[row][0];
            tmp2[row][0].type = tmp2[row][0].color = ' ';

            add_castling_move(tmp2, moveList, parent, player, /*kingside=*/0);
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
        child->whiteKingsideRookMoved = kingside ? 1 : parent_state->whiteKingsideRookMoved;
        child->whiteQueensideRookMoved = kingside ? parent_state->whiteQueensideRookMoved : 1;
    } else {
        child->blackKingMoved = 1;
        child->blackKingsideRookMoved = kingside ? 1 : parent_state->blackKingsideRookMoved;
        child->blackQueensideRookMoved = kingside ? parent_state->blackQueensideRookMoved : 1;
    }

    child->next = *list;
    *list = child;
}



void handlePromotion(Piece board[8][8], int i2, int j2, char player) {
    if (player == 'w' && i2 == 0 && board[i2][j2].type == 'P') {
        printf("DEBUG: promotion blanc en (%d,%d)\n", i2, j2);
        board[i2][j2].type = 'Q';
    }
    else if (player == 'b' && i2 == 7 && board[i2][j2].type == 'p') {
        printf("DEBUG: promotion noir en (%d,%d)\n", i2, j2);
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

