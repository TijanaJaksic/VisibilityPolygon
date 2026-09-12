#include <SFML/Graphics.hpp>
#include "geometry.hpp"
#include "visibility.hpp"


int main()
{
	sf::RenderWindow window( sf::VideoMode( { 1000, 1000 } ), "SFML works!" );

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

	std::vector<geometry::Segment> segments = {
    // Internal segments
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
    // {{311, 280}, {258, 172}},
    {{784, 134}, {722, 788}},
    {{462, 62}, {60, 125}},
    {{695, 748}, {588, 459}},


    // Frame
    {{10, 10}, {10, 990}},
    {{10, 990}, {990, 990}},
    {{990, 990}, {990, 10}},
    {{990, 10}, {10, 10}}
};

	

	geometry::Polygon polygon(visibilityPolygon(viewpoint, segments, window));

	while ( window.isOpen() )
	{
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

		window.clear();

		
		for(auto segment: segments){
			segment.draw(window);
		}

		polygon.draw(window, sf::Color(200, 200, 220));
		polygon.drawPoints(window, sf::Color::Red);
		
		viewpoint.draw(window);

		window.display();
	}
	for(auto point: polygon.vertices){
		std::cout<<point<<std::endl;
	}
}
