#ifndef GEOMETRY_
#define GEOMETRY_


#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include <vector>

const float PI = acos(-1.0);
const float INF = 1e100;
const float EPS = 1e-6f;


namespace geometry
{
    struct Point{
        float x, y;

        bool operator==(const Point& p) const {
            return abs(x-p.x) < EPS && abs(y-p.y) < EPS;
        }


        Point operator+(const Point& p) const {
            return {x + p.x, y + p.y};
        }

        Point operator-(const Point& p) const {
            return {x - p.x, y - p.y};
        }

        Point operator*(float t) const {
            return {x * t, y * t};
        }

        float distance() const {
            return x*x + y*y;
        }

        friend std::ostream& operator<<(std::ostream& os, const Point& p) {
            os << "(" << p.x << ", " << p.y << ")";
            return os;
        }

        // todo have color be one of the arguments
        void draw(sf::RenderWindow& window) const {
            sf::CircleShape circle( 10.f );
            circle.setPosition({x, y}); 
            circle.setFillColor( sf::Color::White );
            window.draw(circle);
        }
    };

    float cross(Point a, Point b) {
        return a.x * b.y - a.y * b.x;
    }
    // TODO: MAYBE DELETE
    // struct comparePoints{
    //     bool operator()(const Point& a, const Point& b, const Point& viewpoint){

    //     }
    // };

    struct Segment {
        Point a;
        Point b;

        bool operator==(const Segment &other) const {
            return a.x == other.a.x && a.y == other.a.y && b.x == other.b.x && b.y == other.b.y;
        }

        friend std::ostream& operator<<(std::ostream& os, const Segment& segment) {
        os << "[" << segment.a << ", " << segment.b << "]";
        return os;
        }

        // void draw(sf::RenderWindow& window) const {
        //     std::vector<sf::Vertex> line =
        //     {
        //         sf::Vertex{sf::Vector2f(a.x, a.y)},
        //         sf::Vertex{sf::Vector2f(b.x, b.y)}
        //     };
        //     window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        // }
        void draw(sf::RenderWindow& window) const {
            sf::Vector2f start(a.x, a.y);
            sf::Vector2f end(b.x, b.y);

            sf::Vector2f direction = end - start;

            float length = std::sqrt(direction.x * direction.x +
                                    direction.y * direction.y);

            float angle = std::atan2(direction.y, direction.x) * 180.f / PI;

            sf::RectangleShape line({length, 5.f});

            line.setOrigin({0.f, 2.5f});
            line.setPosition(start);
            line.setRotation(sf::degrees(angle));
            line.setFillColor(sf::Color(180, 180, 180)); // grey

            window.draw(line);
        }
    };

    // provera orijentacije 
    int is_ccw(const Point& p, const Point& q, const Point& r){
        float cross_product = (q.y-p.y)*(r.x-p.x) - (r.y-p.y)*(q.x-p.x);
        if (cross_product > 0) return 1;
        if (cross_product < 0) return -1;
        return 0;
    }

    struct Polygon {
        std::vector<Point> vertices;

        Polygon(std::vector<Point> vertices) : vertices(std::move(vertices)) {}

        void draw(sf::RenderWindow& window) const {
            sf::ConvexShape polygon;
            polygon.setPointCount(vertices.size());

            for(int i = 0; i < vertices.size(); i++){
                polygon.setPoint(i, {sf::Vector2f(vertices[i].x, vertices[i].y)});
            }

            polygon.setFillColor(sf::Color::Yellow);
            window.draw(polygon);
        }
    };
}

#endif // GEOMETRY_