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

// global var, current sweep angle
float sweepAngle = 0.0;

Point origin = {0.0, 0.0};

struct Event {
    float angle;
    Point point;
    const Segment segment;
    bool endPoint;
};

struct CompareEvents {
    bool operator()(const Event& e1, const Event& e2) const {

        // 1. Smaller angle comes first
        if (std::fabs(e1.angle - e2.angle) > EPS)
            return e1.angle < e2.angle;

        // 2. Same angle: farther point comes first
        float d1 = (e1.point - origin).distance();
        float d2 = (e2.point - origin).distance();

        if (std::fabs(d1 - d2) > EPS)
            return d1 > d2;

        // 3. Same point/distance: start points come first
        // endPoint == false -> start
        // endPoint == true  -> end
        if (e1.endPoint != e2.endPoint)
            return !e1.endPoint;

        // 4. Same type: segment whose OTHER endpoint is farther
        // comes first.
        Point other1;
        Point other2;

        if (e1.point == e1.segment.a)
            other1 = e1.segment.b;
        else
            other1 = e1.segment.a;

        if (e2.point == e2.segment.a)
            other2 = e2.segment.b;
        else
            other2 = e2.segment.a;

        float otherD1 = (other1 - origin).distance();
        float otherD2 = (other2 - origin).distance();

        if (std::fabs(otherD1 - otherD2) > EPS)
            return otherD1 > otherD2;

        // 5. Everything is equivalent
        return false;
    }
};

float distanceToRay(const Segment& s) {
    // vektor usmerenja zraka
    Point d = {std::cos(sweepAngle), std::sin(sweepAngle)};
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

/*
    Presek segmenta sa sweep zrakom.
*/
Point intersectionWithRay(const Segment& s, sf::RenderWindow& window) {
    Point d = {std::cos(sweepAngle), std::sin(sweepAngle)};
    float t = distanceToRay(s);
    if (t == INF)
        return {-1, -1}; // shouldn't happen
    return origin + d * t;
}

struct CompareSegment {
    bool operator()(const Segment& s1, const Segment& s2) const {
        if (s1 == s2)
            return false;

        float d1 = distanceToRay(s1);
        float d2 = distanceToRay(s2);

        if (fabs(d1 - d2) > EPS)
            return d1 < d2;

        // Tie-breaker: lexicographical ordering of endpoints
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
    std::set<Event, CompareEvents> events;

    // Svaki endpoint ubacimo u red dogadjaja:
    for (const Segment& s : segments) {
        float a1 = atan2(s.a.y - origin.y, s.a.x - origin.x);

        float a2 = atan2(s.b.y - origin.y, s.b.x - origin.x);

        if (orientation(origin, s.a, s.b) == 1) {
            events.insert({a1, s.a, s, false});
            events.insert({a2, s.b, s, true});
        } else {
            events.insert({a2, s.b, s, false});
            events.insert({a1, s.a, s, true});
        }
    }

    // aktivni segmenti:
    std::set<Segment, CompareSegment> status;

    // pocetni ugao
    sweepAngle = (*events.begin()).angle - 0.0001;

    // Ubacujemo segmente koji presecaju početni zrak - pravimo pocetni status
    for (const Segment& s : segments) {
        if (distanceToRay(s) != INF)
            status.insert(s);
    }

    std::vector<geometry::Point> result;

    // Prolazak kroz dogadjaje
    while (!events.empty()) {
        Event e = *events.begin();
        sweepAngle = e.angle;
        events.erase(*events.begin());

        if (e.endPoint) {                       // ako je end point
            if (e.segment == *status.begin()) { // ako je segment najblizi, onda je vidljiv
                result.push_back(e.point);      // dodaj tacku u rez
                status.erase(e.segment);        // ukloni segment iz statusa
                if (!status
                         .empty()) { // ako nakon uklanjanja, status nije prazan, imamo novi presek
                    if (distanceToRay(*status.begin()) == INF)
                        continue;
                    Point newIntersection = intersectionWithRay(*status.begin(), window);
                    std::cout << "endPoint " << newIntersection << ": " << *status.begin()
                              << std::endl;
                    result.push_back(newIntersection); // dodaj novi presek u rez
                }
            } else {                     // ako nije najblizi
                status.erase(e.segment); // samo ukloni iz statusa
            }
        } else { // ako nije end point, onda je start point.
            Point newIntersection;
            bool intersectionAdded = false;
            if (!status.empty()) {
                if (distanceToRay(*status.begin()) == INF)
                    continue;
                newIntersection = intersectionWithRay(
                    *status.begin(),
                    window); // presek sa segmentom najblizim originu, pre dodavanja naseg segmenta
                intersectionAdded = true;
                std::cout << "startPoint " << newIntersection << ": " << *status.begin()
                          << std::endl;
            }
            status.insert(e.segment);           // dodaj segment u status
            if (e.segment == *status.begin()) { // ako je ovo novi najblizi element
                // dodaj novi presek i pocetnu tacku u visibility poligon
                if (intersectionAdded) {
                    result.push_back(newIntersection);
                }
                result.push_back(e.point);
            } // ako nije, nista
        }
    }
    return result;
}

#endif // VISIBILITY_