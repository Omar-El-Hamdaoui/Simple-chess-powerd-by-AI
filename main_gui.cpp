#include <SFML/Graphics.hpp>
#include <map>
#include <string>

const int SQUARE_SIZE = 75;
const int BOARD_SIZE = 8;

// Utilisation de lettres standards pour les pièces
const char board[8][8] = {
    {'r','n','b','q','k','b','n','r'},
    {'p','p','p','p','p','p','p','p'},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {'P','P','P','P','P','P','P','P'},
    {'R','N','B','Q','K','B','N','R'},
};

std::string getImageFilename(char piece) {
    if (piece == ' ') return "";
    std::string filename;
    filename += (isupper(piece) ? 'w' : 'b');
    filename += tolower(piece);
    return "img/" + filename + ".png";
}

int main() {
    sf::RenderWindow window(sf::VideoMode(SQUARE_SIZE * BOARD_SIZE, SQUARE_SIZE * BOARD_SIZE), "MiniChess - Echiquier");

    std::map<char, sf::Texture> textures;
    std::map<char, sf::Sprite> sprites;

    // Charger les images
    std::string allPieces = "rnbqkpRNBQKP";
    for (char p : allPieces) {
        std::string path = getImageFilename(p);
        if (!textures[p].loadFromFile(path)) {
            printf("Erreur chargement image : %s\n", path.c_str());
            return 1;
        }
        sprites[p].setTexture(textures[p]);
        sprites[p].setScale(
            float(SQUARE_SIZE) / textures[p].getSize().x,
            float(SQUARE_SIZE) / textures[p].getSize().y
        );
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                sf::RectangleShape square(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                square.setPosition(j * SQUARE_SIZE, i * SQUARE_SIZE);
                square.setFillColor((i + j) % 2 == 0 ? sf::Color(240, 217, 181) : sf::Color(181, 136, 99));
                window.draw(square);

                char piece = board[i][j];
                if (piece != ' ') {
                    sprites[piece].setPosition(j * SQUARE_SIZE, i * SQUARE_SIZE);
                    window.draw(sprites[piece]);
                }
            }
        }

        window.display();
    }

    return 0;
}
