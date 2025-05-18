/* chess.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgic.h"
#include "board.h"
#include "movegen.h"
#include "ai.h"
#include "list.h"

#define MAX_FEN 100

static void print_html_header(const char *title){
    cgiHeaderContentType("text/html; charset=UTF-8");
    fprintf(cgiOut,
        "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
        "<title>%s</title>"
        "<style> table {border-collapse:collapse;} "
        "td {width:40px;height:40px;text-align:center;"
        "font-size:32px;} </style>"
        "</head><body>\n", title);
}

static wchar_t piece_unicode(Piece p){
    switch (p.type) {
        case 'K': return 0x2654; // ♔
        case 'Q': return 0x2655; // ♕
        case 'R': return 0x2656; // ♖
        case 'B': return 0x2657; // ♗
        case 'N': return 0x2658; // ♘
        case 'P': return 0x2659; // ♙
        case 'k': return 0x265A; // ♚
        case 'q': return 0x265B; // ♛
        case 'r': return 0x265C; // ♜
        case 'b': return 0x265D; // ♝
        case 'n': return 0x265E; // ♞
        case 'p': return 0x265F; // ♟︎
        default:  return L'·';
    }
}


int cgiMain() {
    char fen[MAX_FEN] = "";
    cgiFormStringNoNewlines("fen", fen, MAX_FEN);
    if (fen[0]==0) {
        // position de départ FEN standard
        strcpy(fen, "rn.../pppp.../8/8/8/8/PPPP.../RN... w KQkq - 0 1");
        // remplacez par votre initBoard SQL ou générez FEN initial
    }
    // reconstruire le board depuis fen
    Piece board[8][8];
    fenToBoard(fen, board);

    print_html_header("MiniChess CGI");

    fprintf(cgiOut, "<h1>MiniChess</h1>\n");

    // Formulaire pour chaque case : clic sur un carré = sélection/source ou destination
    // On mémorise `src` et `dst` dans deux champs cachés
    char src[4]="", dst[4]="";
    cgiFormStringNoNewlines("src", src, 4);
    cgiFormStringNoNewlines("dst", dst, 4);

    // Si on a déjà src+dst → on joue le coup
    if (src[0] && dst[0]) {
        // essayer de jouer
        Item* moves = generateMoves(board, src[0]);
        // compare src,dst notation algébrique, applique le premier qui matche
        // puis freeList et met à jour board[]
        freeList(moves);
        strcpy(src,""); strcpy(dst,"");
    }

    // ré-affiche le board
    fprintf(cgiOut, "<form method='get'>\n");
    // champs cachés
    fprintf(cgiOut,
      "<input type='hidden' name='fen' value='%s'/>\n"
      "<input type='hidden' name='src' value='%s'/>"
      "<input type='hidden' name='dst' value='%s'/>\n",
      fen, src, dst);

    fprintf(cgiOut, "<table border='1'>\n");
    for (int i=0;i<8;i++){
      fprintf(cgiOut,"<tr>");
      for(int j=0;j<8;j++){
        char sq[3] = { 'a'+j, '8'-i, 0 };
        fprintf(cgiOut,
          "<td><button name='%s' value='%s' style='width:100%%;height:100%%;"
          "background:none;border:none;font-size:28px;'>%lc</button></td>",
          src[0] ? "dst" : "src", sq, piece_unicode(board[i][j]));
      }
      fprintf(cgiOut,"</tr>\n");
    }
    fprintf(cgiOut,"</table>\n");
    fprintf(cgiOut,"</form>\n");

    fprintf(cgiOut, "</body></html>\n");
    return 0;
}
