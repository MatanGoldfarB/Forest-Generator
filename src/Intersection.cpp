#include "Intersection.h"
#include "Scene.h"
#include "Light.h"
#include "Ray.h"

// Intersection class to store hit details

Intersection :: ~Intersection() {}

Intersection :: Intersection() : hit(false), distance(std::numeric_limits<float>::max()) {}
Intersection :: Intersection(bool h, float d, Vector p, Vector n, Vector c, float s, bool r, bool t) : hit(h), distance(d), point(p), normal(n), color(c), shininess(s), reflective(r), transparent(t){}
bool Intersection :: getHit(){
    return hit;
}
