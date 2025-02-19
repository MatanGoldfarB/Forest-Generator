// Ray.h
#ifndef RAY_H
#define RAY_H

#include <limits>
#include "Vector.h"

class Ray {
public:
    Vector origin;
    Vector direction;

    Ray(const Vector& o, const Vector& d);
};

#endif // RAY_H