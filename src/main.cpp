#include <SFML/Graphics.hpp>
#include "geometry.hpp"
#include "visibility.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 1000}), "Visibility polygon");

    geometry::Point viewpoint = {500, 500};

    // linije:
    // std::vector<geometry::Segment> segments = {
    // 	{{100, 120}, {300, 200}},
    // 	{{500, 100}, {800, 150}},
    // 	{{150, 400}, {400, 600}},
    // 	{{600, 350}, {850, 700}},
    // 	{{200, 850}, {700, 800}},
    // 	{{10, 10}, {10, 990}},
    // 	{{10, 990}, {990, 990}},
    // 	{{990, 990}, {990, 10}},
    // 	{{990, 10}, {10, 10}}
    // };

    std::vector<geometry::Segment> segments = {// Internal segments
                                               {{858, 920}, {36, 807}},
                                               {{378, 314}, {189, 250}},
                                               {{74, 777}, {500, 579}},

                                               {{157, 417}, {110, 595}},
                                               {{111, 905}, {268, 917}},
                                               {{604, 581}, {299, 794}},
                                               {{936, 579}, {798, 303}},
                                               {{330, 475}, {191, 494}},
                                               {{549, 964}, {138, 921}},
                                               {{117, 779}, {527, 865}},
                                               {{368, 102}, {556, 273}},

                                               {{684, 144}, {55, 789}},
                                               //    {{311, 280}, {258, 172}},
                                               {{784, 134}, {722, 788}},
                                               {{462, 62}, {60, 125}},
                                               {{695, 748}, {588, 459}},

                                               // Frame
                                               {{10, 10}, {10, 990}},
                                               {{10, 990}, {990, 990}},
                                               {{990, 990}, {990, 10}},
                                               {{990, 10}, {10, 10}}};

    std::vector<geometry::Segment> segments2 = {// Internal segments
                                                {{80, 100}, {280, 140}},
                                                {{350, 100}, {550, 160}},
                                                {{650, 100}, {900, 130}},

                                                {{120, 220}, {300, 280}},
                                                {{450, 200}, {700, 260}},
                                                {{780, 220}, {930, 300}},

                                                {{60, 360}, {250, 320}},
                                                {{350, 350}, {520, 400}},
                                                {{620, 330}, {850, 390}},

                                                {{100, 480}, {300, 550}},
                                                {{420, 470}, {650, 530}},
                                                {{750, 460}, {920, 540}},

                                                {{70, 650}, {250, 590}},
                                                {{350, 620}, {550, 700}},
                                                {{650, 640}, {850, 590}},

                                                {{150, 800}, {350, 870}},

                                                // Frame
                                                {{10, 990}, {10, 10}},
                                                {{990, 990}, {10, 990}},
                                                {{990, 10}, {990, 990}},
                                                {{10, 10}, {990, 10}}};

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            // Check if the left mouse button was clicked
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                // Check if it was the left mouse button
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    // Extract the X and Y coordinates
                    viewpoint.x = mousePressed->position.x;
                    viewpoint.y = mousePressed->position.y;
                }
            }
        }

        window.clear();

        geometry::Polygon polygon(visibilityPolygon(viewpoint, segments2, window));

        geometry::Segment s = {{311, 280}, {258, 172}};
        // s.draw(window, sf::Color::Green);

        viewpoint.draw(window);
        polygon.drawPoints(window);

        for (auto segment : segments2) {
            segment.draw(window);
        }

        window.display();
    }
}
