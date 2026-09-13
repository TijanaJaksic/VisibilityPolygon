#ifndef VISIBILITY_
#define VISIBILITY_

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include <vector>
#include "geometry.hpp"

using namespace geometry;
#define ANGLE_EPS 0.001

// global var, current sweep angle
float sweepAngle = 0.0;

Point origin = {0.0, 0.0};

struct Event {
    float angle;
    Point point;
    Segment segment;
    bool endPoint;
};

struct CompareEvents { // razliciti eventi mogu biti ekvivalentnog prioriteta
    bool operator()(const Event& e1, const Event& e2) const {

        // smaller angle comes first
        if (std::fabs(e1.angle - e2.angle) > EPS)
            return e1.angle < e2.angle;

        // same angle - farther point comes first
        float d1 = (e1.point - origin).distance();
        float d2 = (e2.point - origin).distance();

        if (std::fabs(d1 - d2) > EPS)
            return d1 > d2;

        // same point&distance: end points come first
        // endPoint == false -> start
        // endPoint == true  -> end
        if (e1.endPoint != e2.endPoint)
            return e1.endPoint;

        // 5. Everything is equivalent
        return false;
    }
};

float distanceToRay(const Segment& s, float angle) {
    // vektor usmerenja zraka
    Point d = {std::cos(angle), std::sin(angle)};
    // ray is origin + t*d, t>=0

    // vektor segmenta
    Point v = s.b - s.a;
    // segment is s.a + u*v, u \in [0, 1]

    // ako su paralelne, ne trazimo presek dalje
    float denominator = cross(d, v);
    if (std::fabs(denominator) < EPS)
        return INF;

    /*
        origin + t*d = s.a + u*v
        t*d = (s.a-origin) + u*v
        t*cross(d, v) = cross(s.a-origin, v)
        t = cross(w, v)/cross(d, v)

        spicno:
        u = cross(w, d) / cross(d, v)
    */
    Point w = s.a - origin;

    float t = cross(w, v) / denominator;
    float u = cross(w, d) / denominator;

    if (t < -EPS) // ako je t na zraku, t je u [0, INF]
        return INF;

    if (u < -EPS || u > 1.0 + EPS) // ako je u na segmentu, u je u [0, 1]
        return INF;

    return std::fmax(0.0, t); // t predstavlja euklidsko rastojanje od origina do preseka
}

// presek segmenta sa sweep zrakom
Point intersectionWithRay(const Segment& s, sf::RenderWindow& window) {
    Point d = {std::cos(sweepAngle), std::sin(sweepAngle)};
    float t = distanceToRay(s, sweepAngle);
    if (t == INF)
        return {-1, -1}; // shouldn't happen
    return origin + d * t;
}

static bool isColinearWithOrigin(const Segment& s) {
    return orientation(s.a, s.b, origin) == 0;
}

struct CompareSegment {
    bool operator()(const Segment& s1, const Segment& s2) const {
        // da bi std::set funkcionisao, treba da poredim segmente nezavisno od sweep zraka, samo na
        // osnovu viewpointa
        if (s1 == s2)
            return false;

        Point a1, b1, a2, b2;
        // uredimo tacke tako da a = START, b = END
        if (orientation(origin, s1.a, s1.b) == 1) {
            // a je start, b je end
            a1 = s1.a;
            b1 = s1.b;
        } else {
            a1 = s1.b;
            b1 = s1.a;
        }
        if (orientation(origin, s2.a, s2.b) == 1) {
            a2 = s2.a;
            b2 = s2.b;
        } else {
            a2 = s2.b;
            b2 = s2.a;
        }

        float angle_a1 = atan2(a1.y - origin.y, a1.x - origin.x);
        float angle_b1 = atan2(b1.y - origin.y, b1.x - origin.x);
        float angle_a2 = atan2(a2.y - origin.y, a2.x - origin.x);
        float angle_b2 = atan2(b2.y - origin.y, b2.x - origin.x);

        if (distanceToRay(s1, angle_a1 + EPS) != INF && distanceToRay(s2, angle_a1 + EPS) != INF)
            return distanceToRay(s1, angle_a1 + EPS) < distanceToRay(s2, angle_a1 + EPS);

        if (distanceToRay(s1, angle_b1 - EPS) != INF && distanceToRay(s2, angle_b1 - EPS) != INF)
            return distanceToRay(s1, angle_b1 - EPS) < distanceToRay(s2, angle_b1 - EPS);

        if (distanceToRay(s1, angle_a2 + EPS) != INF && distanceToRay(s2, angle_a2 + EPS) != INF)
            return distanceToRay(s1, angle_a2 + EPS) < distanceToRay(s2, angle_a2 + EPS);

        if (distanceToRay(s1, angle_b2 - EPS) != INF && distanceToRay(s2, angle_b2 - EPS) != INF)
            return distanceToRay(s1, angle_b2 - EPS) < distanceToRay(s2, angle_b2 - EPS);

        // ukoliko leze na istoj pravoj i sa iste strane, ili leze sa raznih strana, ne bi trebalo
        // da je bitno koji poredak koristimo Tie-breaker: lexicographical ordering of endpoints
        if (fabs(s1.a.x - s2.a.x) > EPS)
            return s1.a.x < s2.a.x;

        if (fabs(s1.a.y - s2.a.y) > EPS)
            return s1.a.y < s2.a.y;

        if (fabs(s1.b.x - s2.b.x) > EPS)
            return s1.b.x < s2.b.x;

        return s1.b.y < s2.b.y;
    }
};

// GLAVNI ALGORITAM:
std::vector<Point> visibilityPolygon(Point viewpoint, const std::vector<Segment>& segments,
                                     sf::RenderWindow& window) {
    origin = viewpoint;
    std::vector<Event> events;

    // svaki endpoint ubacimo u red dogadjaja:
    for (const Segment& s : segments) {
        float a1 = atan2(s.a.y - origin.y, s.a.x - origin.x);

        float a2 = atan2(s.b.y - origin.y, s.b.x - origin.x);

        if (fabs(a1 - a2) < EPS)
            continue; // necu ovakve segmente da dodajem u dogadjaje

        if (orientation(origin, s.a, s.b) == 1) {
            events.push_back({a1, s.a, s, false});
            events.push_back({a2, s.b, s, true});
        } else if (orientation(origin, s.a, s.b) == -1) {
            events.push_back({a2, s.b, s, false});
            events.push_back({a1, s.a, s, true});
        }
    }
    // sortiramo dogadjaje:
    std::sort(events.begin(), events.end(), CompareEvents());

    if (events.empty())
        return {};

    // aktivni segmenti:
    std::set<Segment, CompareSegment> status;

    // pocetni ugao
    sweepAngle = (*events.begin()).angle - ANGLE_EPS;

    // Ubacujemo segmente koji presecaju početni zrak - pravimo pocetni status
    for (const Segment& s : segments) {
        if (distanceToRay(s, sweepAngle) != INF && orientation(s.a, s.b, origin) != 0)
            status.insert(s);
    }

    std::vector<geometry::Point> result;

    std::cout << "---------------------------\n";
    // Prolazak kroz dogadjaje i formiranje poligona vidljivosti:
    for (Event e : events) {
        sweepAngle = e.angle;
        std::cout << "\n";
        std::cout << e.point << std::endl;

        std::cout << "status before: ";
        for (auto s : status) {
            std::cout << s << ", ";
        }
        std::cout << "\n";

        if (e.endPoint) {                       // ako je end point
            if (e.segment == *status.begin()) { // ako je segment najblizi, onda je vidljiv
                result.push_back(e.point);      // dodaj tacku u rez
                status.erase(e.segment);        // ukloni segment iz statusa
                if (!status.empty()) { // ako nakon uklanjanja, status nije prazan, imamo novi
                                       // presek
                    if (distanceToRay(*status.begin(), sweepAngle) == INF)
                        continue;
                    Point newIntersection = intersectionWithRay(*status.begin(), window);
                    result.push_back(newIntersection); // dodaj novi presek u rez
                }
            } else {                     // ako nije najblizi
                status.erase(e.segment); // samo ukloni iz statusa
            }
        } else { // ako nije end point, onda je start point.
            Point newIntersection;
            bool intersectionAdded = false;
            if (!status.empty()) {
                if (distanceToRay(*status.begin(), sweepAngle) != INF) {
                    newIntersection =
                        intersectionWithRay(*status.begin(),
                                            window); // presek sa segmentom najblizim originu,
                                                     // pre dodavanja naseg segmenta
                    intersectionAdded = true;
                }
            }
            if (orientation(e.segment.a, e.segment.b, origin) != 0)
                status.insert(e.segment);       // dodaj segment u status
            if (e.segment == *status.begin()) { // ako je ovo novi najblizi element
                // dodaj novi presek i pocetnu tacku u visibility poligon
                if (intersectionAdded) {
                    result.push_back(newIntersection);
                }
                result.push_back(e.point);
            } // ako nije, nista
        }

        std::cout << "status after: ";
        for (auto s : status) {
            std::cout << s << ", ";
        }
        std::cout << "\n";
    }

    // DRAW THE POLYGON:
    if (result.size() < 2)
        return result;

    for (int i = 0; i < result.size() - 1; i += 1) {
        // draw triangle {result[i], result[i+1], origin}
        // should work even if triangle is degenerated
        sf::ConvexShape triangle;
        triangle.setPointCount(3);
        if (orientation(result[i], result[i + 1], origin) == 0)
            continue;
        std::vector<Point> points = {result[i], result[i + 1], origin};
        for (int j = 0; j < 3; j++) {
            triangle.setPoint(j, {sf::Vector2f(points[j].x, points[j].y)});
        }
        triangle.setFillColor(sf::Color(255, 255, 100, 150));
        window.draw(triangle);
    }
    // draw the last triangle {result[0], result[end], origin}
    sf::ConvexShape triangle;
    triangle.setPointCount(3);
    std::vector<Point> points = {result[0], result[result.size() - 1], origin};
    for (int i = 0; i < 3; i++) {
        triangle.setPoint(i, {sf::Vector2f(points[i].x, points[i].y)});
    }
    triangle.setFillColor(sf::Color(255, 255, 100, 150));
    window.draw(triangle);

    return result;
}

#endif // VISIBILITY_