#ifndef GEOMETRY_
#define GEOMETRY_

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include <vector>
#include <limits>

const float PI = acos(-1.0);
const float INF = std::numeric_limits<float>::infinity();
const float EPS = 1e-4f;

namespace geometry {
struct Point {
    float x, y;

    bool operator==(const Point& p) const {
        return fabs(x - p.x) < EPS && fabs(y - p.y) < EPS;
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
        return x * x + y * y;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        os << "(" << p.x << ", " << p.y << ")";
        return os;
    }

    // todo have color be one of the arguments
    void draw(sf::RenderWindow& window, sf::Color color = sf::Color::White,
              float radius = 5.f) const {
        sf::CircleShape circle(radius);
        circle.setPosition({x - radius, y - radius});
        circle.setFillColor(color);
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

    bool operator==(const Segment& other) const {
        return a == other.a && b == other.b;
    }

    void draw(sf::RenderWindow& window, sf::Color color = sf::Color(180, 180, 180),
              float thickness = 3.f) const {
        sf::Vector2f start(a.x, a.y);
        sf::Vector2f end(b.x, b.y);

        sf::Vector2f direction = end - start;

        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        float angle = std::atan2(direction.y, direction.x) * 180.f / PI;

        sf::RectangleShape line({length, thickness});

        line.setOrigin({0.f, thickness / 2.f});
        line.setPosition(start);
        line.setRotation(sf::degrees(angle));
        line.setFillColor(color); // grey

        window.draw(line);
    }
    friend std::ostream& operator<<(std::ostream& os, const Segment& s) {
        os << "{" << s.a << ", " << s.b << "}";
        return os;
    }
};

// provera orijentacije
int orientation(const Point& p, const Point& q, const Point& r) {
    float cross_product = (q.y - p.y) * (r.x - p.x) - (r.y - p.y) * (q.x - p.x);
    if (cross_product > EPS)
        return -1;
    if (cross_product < -EPS)
        return 1;
    return 0;
}

struct Polygon {
    std::vector<Point> vertices;

    Polygon(std::vector<Point> vertices) : vertices(std::move(vertices)) {}

    void drawOutline(sf::RenderWindow& window, sf::Color color) const {
        if (vertices.empty())
            return;

        std::vector<sf::Vertex> line;
        for (const auto& point : vertices) {
            line.push_back(sf::Vertex{{point.x, point.y}, color});
        }
        if (vertices.size() > 2) {
            line.push_back(sf::Vertex{{vertices[0].x, vertices[0].y}, color});
        }

        if (!line.empty()) {
            window.draw(line.data(), line.size(), sf::PrimitiveType::LineStrip);
        }
    }

    void drawPoints(sf::RenderWindow& window, sf::Color color = sf::Color::Red,
                    float radius = 5.0f) const {
        for (const auto& point : vertices) {
            sf::CircleShape circle(radius);
            circle.setFillColor(color);
            circle.setPosition({point.x - radius, point.y - radius});
            window.draw(circle);
        }
    }
};
} // namespace geometry

#endif // GEOMETRY_