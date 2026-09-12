#include <SFML/Graphics.hpp>
#include "geometry.hpp"
#include "visibility.hpp"


int main()
{
	sf::RenderWindow window( sf::VideoMode( { 1000, 1000 } ), "SFML works!" );

	geometry::Point viewpoint = {500, 500};

	// linije:
	std::vector<geometry::Segment> segments = {
		{{100, 100}, {300, 200}},
		{{500, 100}, {800, 150}},
		{{150, 400}, {400, 600}},
		{{600, 350}, {850, 700}},
		{{200, 850}, {700, 800}},
		{{0, 0}, {0, 1000}},
		{{0, 1000}, {1000, 1000}},
		{{1000, 1000}, {1000, 0}},
		{{1000, 0}, {0, 0}}
	};

	geometry::Polygon polygon(visibilityPolygon(viewpoint, segments));


	while ( window.isOpen() )
	{
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

		window.clear();

		polygon.draw(window);
		for(auto segment: segments){
			segment.draw(window);
		}
		
		viewpoint.draw(window);

		window.display();
	}
}
