#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <cctype>

extern "C" {
    #include "board.h"
    #include "movegen.h"
    #include "ai.h"
    #include "list.h"
}

const int TILE_SIZE  = 80;

// Charge la texture par caractère de pièce
sf::Texture loadPieceTexture(char piece) {
    sf::Texture texture;
    std::string path = "img/";
    path += (std::isupper(piece) ? 'w' : 'b');
    path += std::tolower(piece);
    path += ".png";
    if (!texture.loadFromFile(path)) {
        std::cerr << "Erreur de chargement: " << path << "\n";
    }
    return texture;
}

// Dessine le plateau + pièces + highlights
void drawBoard(sf::RenderWindow& window,
               Piece board[BOARD_SIZE][BOARD_SIZE],
               std::map<char, sf::Texture>& textures,
               bool selected[BOARD_SIZE][BOARD_SIZE],
               bool legal[BOARD_SIZE][BOARD_SIZE])
{
    window.clear(sf::Color::White);

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            sf::RectangleShape sq({float(TILE_SIZE), float(TILE_SIZE)});
            sq.setPosition(j * TILE_SIZE, i * TILE_SIZE);

            if (selected[i][j]) {
                sq.setFillColor(sf::Color(50,200,50,180));
            } else if (legal[i][j]) {
                sq.setFillColor(sf::Color(200,200,50,120));
            } else {
                bool light = ((i + j) % 2) == 0;
                sq.setFillColor(
                  light
                    ? sf::Color(240,217,181)
                    : sf::Color(181,136,99)
                );
            }
            window.draw(sq);

            char t = board[i][j].type;
            if (t != ' ') {
                sf::Sprite spr;
                spr.setTexture(textures[t]);
                auto sz = spr.getTexture()->getSize();
                spr.setOrigin(sz.x/2.f, sz.y/2.f);
                spr.setPosition(
                  j * TILE_SIZE + TILE_SIZE/2.f,
                  i * TILE_SIZE + TILE_SIZE/2.f
                );
                window.draw(spr);
            }
        }
    }
    window.display();
}

int main() {
    // Ces deux variables remplacent toute lecture de board[sel_i][sel_j]
    char selType  = ' ';
    char selColor = ' ' ;

    sf::RenderWindow window(
        sf::VideoMode(TILE_SIZE * BOARD_SIZE,
                      TILE_SIZE * BOARD_SIZE),
        "MiniChess SFML - Joueur vs IA"
    );

    // --- 1) Initialise le plateau ---
    Piece board[BOARD_SIZE][BOARD_SIZE];
    initBoard(board);
    char currentPlayer = 'w';  // le joueur humain contrôle les blancs

    // --- 2) Charge les textures ---
    std::map<char, sf::Texture> textures;
    std::vector<char> pieces = {
        'P','R','N','B','Q','K',
        'p','r','n','b','q','k'
    };
    for (char p : pieces) {
        textures[p] = loadPieceTexture(p);
    }

    // --- 3) Variables de sélection & highlights ---
    bool haveSelection = false;
    int  sel_i = -1, sel_j = -1;
    bool legal[BOARD_SIZE][BOARD_SIZE]    = {{false}};
    bool selected[BOARD_SIZE][BOARD_SIZE] = {{false}};

    // Affichage initial
    drawBoard(window, board, textures, selected, legal);

    // Boucle principale
    while (window.isOpen()) {
        // ─ Gestion des événements ───────────────────────
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::MouseButtonPressed &&
                     event.mouseButton.button == sf::Mouse::Left &&
                     currentPlayer == 'w')
            {
                int x = event.mouseButton.x / TILE_SIZE;
                int y = event.mouseButton.y / TILE_SIZE;

                // 1er clic : on choisit ou on re-sélectionne une pièce blanche
                if (!haveSelection) {
                    if (x>=0 && x<BOARD_SIZE && y>=0 && y<BOARD_SIZE &&
                        board[y][x].type!=' ' && board[y][x].color=='w')
                    {
                        haveSelection = true;
                        sel_i = y; sel_j = x;
                        selType  = board[y][x].type;
                        selColor = board[y][x].color;
                    }
                }
                else {
                    // si on clique sur une autre pièce blanche => on change de sel
                    if (x>=0 && x<BOARD_SIZE && y>=0 && y<BOARD_SIZE &&
                        board[y][x].type!=' ' && board[y][x].color=='w')
                    {
                        sel_i = y; sel_j = x;
                        selType  = board[y][x].type;
                        selColor = board[y][x].color;
                    }
                    // clic sur une case vide **et** légale => on joue
                    else if (x>=0 && x<BOARD_SIZE && y>=0 && y<BOARD_SIZE && legal[y][x])
                    {
                        bool valid = false;
                        Item* moves = generateMoves(board,'w');
                        for (Item* m = moves; m; m = m->next) {
                            if ( m->board[sel_i][sel_j].type==' ' &&
                                 m->board[y][x].type  == selType &&
                                 m->board[y][x].color == selColor )
                            {
                                std::memcpy(board, m->board, sizeof(board));
                                valid = true;
                                break;
                            }
                        }
                        freeList(moves);

                        haveSelection = false;
                        std::memset(selected,0,sizeof(selected));
                        std::memset(legal,   0,sizeof(legal));

                        if (valid) {
                            drawBoard(window, board, textures, selected, legal);
                            sf::sleep(sf::milliseconds(150));
                            currentPlayer = 'b';
                        }
                    }
                    // clic ailleurs => on annule la sélection
                    else {
                        haveSelection = false;
                        std::memset(selected,0,sizeof(selected));
                        std::memset(legal,   0,sizeof(legal));
                    }
                }
            }
        }

        // ─ Tour de l'IA ───────────────────────────────────
        if (currentPlayer == 'b') {
            Item* best = chooseBestMove(board, 'b', 2);
            if (best) {
                std::memcpy(board, best->board, sizeof(board));
                free(best);
            }
            currentPlayer = 'w';
            drawBoard(window, board, textures, selected, legal);
            sf::sleep(sf::milliseconds(150));
        }

        // ─ Recalcul des highlights si sélection active ─────
        if (haveSelection) {
            std::memset(legal,    0, sizeof(legal));
            std::memset(selected, 0, sizeof(selected));
            selected[sel_i][sel_j] = true;

            Item* moves = generateMoves(board, 'w');
            for (Item* m = moves; m; m = m->next) {
                // le coup doit vider la case d'origine
                if (m->board[sel_i][sel_j].type != ' ') continue;

                // ne marquer que la case vide→occupée par la même pièce
                for (int i = 0; i < BOARD_SIZE; ++i) {
                    for (int j = 0; j < BOARD_SIZE; ++j) {
                        bool wasEmpty = (board[i][j].type==' ');
                        bool wasEnemy = (board[i][j].type!=' ' && board[i][j].color=='b');
                        bool isOurPiece = (m->board[i][j].type  == selType &&
                                           m->board[i][j].color == selColor);
                        if ((wasEmpty || wasEnemy) && isOurPiece) {
                            legal[i][j] = true;
                        }
                    }
                }
            }
            freeList(moves);
            drawBoard(window, board, textures, selected, legal);
        }

        // ─ Petit délai pour ne pas boucler à fond ─────────
        sf::sleep(sf::milliseconds(30));
    }

    return 0;
}
