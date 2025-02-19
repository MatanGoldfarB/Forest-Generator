#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "Vector.h"



class Intersection {
public:
    virtual ~Intersection();
    bool hit;
    float distance;
    Vector point;
    Vector normal;
    Vector color;
    float shininess;
    bool reflective;
    bool transparent;

    Intersection();
    Intersection(bool h, float d, Vector p, Vector n, Vector c, float s, bool r, bool t);
    bool getHit();
};

#endif // INTERSECTION_H
