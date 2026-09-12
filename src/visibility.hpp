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

float distanceToRay(const Point& p){
    return (p-origin).distance();
}

struct CompareEvents {
    bool operator()(const Event& e1, const Event& e2) const {

        // 1. Smaller angle comes first
        if (std::fabs(e1.angle - e2.angle) > EPS)
            return e1.angle < e2.angle;

        // 2. Same angle: farther point comes first
        float d1 = distanceToRay(e1.point);
        float d2 = distanceToRay(e2.point);

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

        float otherD1 = distanceToRay(other1);
        float otherD2 = distanceToRay(other2);

        if (std::fabs(otherD1 - otherD2) > EPS)
            return otherD1 > otherD2;

        // 5. Everything is equivalent
        return false;
    }
};


// TODO FIX:
float distanceToRay(const Segment& s) {
    Point d = {std::cos(sweepAngle), std::sin(sweepAngle)};
    Point v = s.b - s.a;

    float denominator = cross(d, v);

    if (std::fabs(denominator) < EPS)
        return INF;

    Point w = s.a - origin;

    /*
        origin + t*d = s.a + u*v

        t = cross(w, v) / cross(d, v)
        u = cross(w, d) / cross(d, v)
    */

    float t = cross(w, v) / denominator;
    float u = cross(w, d) / denominator;

    if (t < -EPS)
        return INF;

    if (u < -EPS || u > 1.0 + EPS)
        return INF;

    return std::fmax(0.0, t);
}

struct CompareSegment {
        bool operator()(const Segment& s1, const Segment& s2) const{
            // TODO check and fix
            if (s1 == s2)
                return false;

            float d1 = distanceToRay(s1);
            float d2 = distanceToRay(s2);

            if (fabs(d1 - d2) > EPS)
                return d1 < d2;

            // todo check and fix
            return false;
        }
};

/*
    Presek segmenta sa sweep zrakom.
*/
Point intersectionWithRay(const Segment& s ) {
        Point d = {cos(sweepAngle), sin(sweepAngle)};

        Point v = s.b - s.a;
        Point w = s.a - origin;

        float denominator = cross(d, v);

        float t = cross(w, v) / denominator;

        return origin + d * t;
}


    /*
        Glavni rotation-sweep algoritam.

        Pretpostavka:
        - segmenti ne presecaju jedni druge
        - segmenti predstavljaju zatvorenu granicu
        - viewpoint je unutar oblasti
    */
std::vector<Point> visibilityPolygon(Point viewpoint, const std::vector<Segment>& segments) {
        origin = viewpoint;
        std::set<Event, CompareEvents> events;

        /*
            Svaki endpoint predstavlja događaj:
            kada sweep zrak prođe kroz njega,
            može da se promeni najbliži segment.
        */
        for (const Segment& s : segments) {
            float a1 = atan2(s.a.y - origin.y,
                            s.a.x - origin.x);

            float a2 = atan2(s.b.y - origin.y,
                            s.b.x - origin.x);

            if(is_ccw(origin, s.a, s.b)){
                events.insert({a1, s.a, s, false});
                events.insert({a2, s.b, s, true});
            } else {
                events.insert({a2, s.b, s, false});
                events.insert({a1, s.a, s, true});
            }
        }

        //aktivni segmenti:
        std::set<Segment, CompareSegment> status;

        /*
            Početni ugao
        */
        sweepAngle = (*events.begin()).angle;

        /*
            Ubacujemo segmente koji presecaju početni zrak - pravimo pocetni status
            TODO check validity
        */
        for (const Segment& s : segments) {
            if (distanceToRay(s) < INF / 2)
                status.insert(s);
        }

        std::vector<geometry::Point> result;

        /*
            Prolazimo kroz sve angularne događaje.
        */
        while(!events.empty()) {
            Event e = *events.begin();
            sweepAngle = e.angle;
            events.erase(*events.begin());

            if(e.endPoint){
                // ako je end point, ukloni segment iz statusa
                
                // ako je bila vidljiva, dodaj je u visibility poligon 
                // i dodaj novi najblizi presek sa zrakom u visibility poligon
                if(e.segment == *status.begin()){
                    result.push_back(e.point);
                    status.erase(e.segment);
                    if (!status.empty()) {
                        Point newIntersection = intersectionWithRay(*status.begin());
                        result.push_back(newIntersection);
                    }
                } else {
                    status.erase(e.segment);
                }
                
            } else {
                // ako nije end point, onda je start point. dodaj segment u status
                status.insert(e.segment);
                // ako je ovo novi najblizi element, dodaj tacku u visibility poligon (ako nije, nista)
                if(e.segment == *status.begin()){
                    result.push_back(e.point);
                }
            }
        }
        return result;
}


#endif // VISIBILITY_