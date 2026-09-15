#include <SFML/Graphics.hpp>
#include "geometry.hpp"
#include "visibility.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 1000}), "Visibility polygon");

    geometry::Point viewpoint = {500, 500};

    auto segments2 = geometry::loadSegmentsFromFile("segments.txt");

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                viewpoint.x = mouseMoved->position.x;
                viewpoint.y = mouseMoved->position.y;
            }
        }

        window.clear();

        geometry::Polygon polygon(visibilityPolygon(viewpoint, segments2, window));

        viewpoint.draw(window);

        for (auto segment : segments2) {
            segment.draw(window, sf::Color(255, 170, 170), 4.0);
        }

        window.display();
    }
}
