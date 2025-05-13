#include <SFML/Graphics.hpp>

const int SQUARE_SIZE = 75;
const int BOARD_SIZE = 8;

int main() {
    sf::RenderWindow window(sf::VideoMode(SQUARE_SIZE * BOARD_SIZE, SQUARE_SIZE * BOARD_SIZE), "MiniChess - Echiquier");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        // Dessiner le damier
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                sf::RectangleShape square(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                square.setPosition(j * SQUARE_SIZE, i * SQUARE_SIZE);

                // Alterner les couleurs
                if ((i + j) % 2 == 0)
                    square.setFillColor(sf::Color(240, 217, 181)); // beige clair
                else
                    square.setFillColor(sf::Color(181, 136, 99));  // marron foncé

                window.draw(square);
            }
        }

        window.display();
    }

    return 0;
}
