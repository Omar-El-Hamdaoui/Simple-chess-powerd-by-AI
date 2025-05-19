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

const int TILE_SIZE = 80;

// Charge une texture pour une pièce donnée
sf::Texture loadPieceTexture(char piece) {
    sf::Texture texture;
    std::string path = "img/";
    path += (std::isupper(piece) ? 'w' : 'b');
    path += std::tolower(piece);
    path += ".png";
    if (!texture.loadFromFile(path)) {
        std::cerr << "Erreur de chargement de la texture pour '" << piece
                  << "' : " << path << "\n";
    }
    return texture;
}

// Dessine le plateau, les pièces et les surbrillances
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
    ai_init();

    // Charge la font système DejaVu Sans
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Impossible de charger la police système !\n";
        return 1;
    }

    sf::RenderWindow window(
        sf::VideoMode(TILE_SIZE * BOARD_SIZE,
                      TILE_SIZE * BOARD_SIZE),
        "MiniChess SFML - Joueur vs IA"
    );

    // Init du plateau
    Piece board[BOARD_SIZE][BOARD_SIZE];
    initBoard(board);
    char currentPlayer = 'w';  // humain = blanc

    // Charge les textures
    std::map<char, sf::Texture> textures;
    std::vector<char> pieces = {
        'P','R','N','B','Q','K',
        'p','r','n','b','q','k'
    };
    for (char p : pieces) {
        textures[p] = loadPieceTexture(p);
    }

    // Sélection & surbrillances
    bool haveSelection = false;
    int sel_i = -1, sel_j = -1;
    bool legal[BOARD_SIZE][BOARD_SIZE]    = {{false}};
    bool selected[BOARD_SIZE][BOARD_SIZE] = {{false}};

    drawBoard(window, board, textures, selected, legal);

    bool gameOver = false;
    std::string endMessage;

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed)
                window.close();

            // clic blanc
            if (ev.type == sf::Event::MouseButtonPressed &&
                ev.mouseButton.button == sf::Mouse::Left &&
                currentPlayer == 'w')
            {
                int x = ev.mouseButton.x / TILE_SIZE;
                int y = ev.mouseButton.y / TILE_SIZE;
                bool inBoard = (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE);

                if (!haveSelection) {
                    if (inBoard && board[y][x].type != ' ' && board[y][x].color == 'w') {
                        haveSelection = true;
                        sel_i = y;
                        sel_j = x;
                    }
                } else {
                    if (inBoard && board[y][x].type != ' ' && board[y][x].color == 'w') {
                        sel_i = y;
                        sel_j = x;
                    }
                    else if (inBoard && legal[y][x]) {
                        bool valid = false;
                        Item* moves = generateMoves(board, 'w');
                        for (Item* m = moves; m; m = m->next) {
                            if (m->board[sel_i][sel_j].type == ' ' &&
                                m->board[y][x].color == 'w')
                            {
                                std::memcpy(board, m->board, sizeof(board));
                                valid = true;
                                break;
                            }
                        }
                        freeList(moves);

                        haveSelection = false;
                        std::memset(selected, 0, sizeof(selected));
                        std::memset(legal,    0, sizeof(legal));

                        if (valid) {
                            // Debug: promotion réelle blanc
                            if (y == 0 && board[y][x].type == 'Q') {
                                std::cout << ">> PROMO réelle blanc en (" << y << "," << x
                                          << ") type=" << board[y][x].type << "\n";
                            }
                            drawBoard(window, board, textures, selected, legal);
                            sf::sleep(sf::milliseconds(150));

                            // test échec & mat / pat
                            char opp = 'b';
                            if (isCheckmate(board, opp)) {
                                endMessage = "Echec et mat, Blancs gagnent !";
                                gameOver = true;
                            } else if (isStalemate(board, opp)) {
                                endMessage = "Pat !";
                                gameOver = true;
                            } else {
                                currentPlayer = 'b';
                            }
                        }
                    } else {
                        haveSelection = false;
                        std::memset(selected, 0, sizeof(selected));
                        std::memset(legal,    0, sizeof(legal));
                    }
                }
            }
        }

        // Tour IA
        if (!gameOver && currentPlayer == 'b') {
            Item* best = chooseBestMove(board, 'b', 3);
            if (best) {
                std::memcpy(board, best->board, sizeof(board));
                // Debug: promotion réelle noir
                for (int i = 0; i < BOARD_SIZE; ++i)
                    for (int j = 0; j < BOARD_SIZE; ++j)
                        if (i == 7 && board[i][j].type == 'q') {
                            std::cout << ">> PROMO réelle noir en (" << i << "," << j << ")\n";
                        }
                free(best);
                drawBoard(window, board, textures, selected, legal);
                sf::sleep(sf::milliseconds(150));

                char opp = 'w';
                if (isCheckmate(board, opp)) {
                    endMessage = "Echec et mat, Noirs gagnent !";
                    gameOver = true;
                } else if (isStalemate(board, opp)) {
                    endMessage = "Pat !";
                    gameOver = true;
                } else {
                    currentPlayer = 'w';
                }
            } else {
                // Aucun coup légal pour les Noirs
                if (isCheckmate(board, 'b'))
                    endMessage = "Échec et mat, Blancs gagnent !";
                else if (isStalemate(board, 'b'))
                    endMessage = "Pat !";
                else
                    endMessage = "Aucun coup possible pour les Noirs.";
                gameOver = true;
                drawBoard(window, board, textures, selected, legal);
            }
        }

        // Surbrillance des coups légaux
        if (!gameOver && haveSelection) {
            std::memset(legal,    0, sizeof(legal));
            std::memset(selected, 0, sizeof(selected));
            selected[sel_i][sel_j] = true;
            Item* moves = generateMoves(board, 'w');
            for (Item* m = moves; m; m = m->next) {
                if (m->board[sel_i][sel_j].type!=' ') continue;
                for (int i = 0; i < BOARD_SIZE; ++i)
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    bool wasEmpty = (board[i][j].type==' ');
                    bool wasEnemy = (board[i][j].type!=' ' && board[i][j].color=='b');
                    bool isOur   = (m->board[i][j].color=='w');
                    if ((wasEmpty||wasEnemy) && isOur)
                        legal[i][j] = true;
                }
            }
            freeList(moves);
            drawBoard(window, board, textures, selected, legal);
        }

        // Fin de partie
        if (gameOver) {
            drawBoard(window, board, textures, selected, legal);
            sf::Text text(endMessage, font, 36);
            text.setFillColor(sf::Color::Red);
            sf::FloatRect bounds = text.getLocalBounds();
            text.setPosition(
                (window.getSize().x - bounds.width)/2 - bounds.left,
                (window.getSize().y - bounds.height)/2 - bounds.top
            );
            window.draw(text);
            window.display();

            sf::Event ev2;
            while (window.isOpen()) {
                while (window.pollEvent(ev2))
                    if (ev2.type == sf::Event::Closed)
                        window.close();
                sf::sleep(sf::milliseconds(100));
            }
        }
    }

    return 0;
}
