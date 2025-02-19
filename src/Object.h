#ifndef OBJECT_H
#define OBJECT_H

#include "Vector.h"
#include "Ray.h"
#include "Intersection.h"



class Object {
public:
    virtual ~Object();
    virtual Intersection intersect(const Ray& ray) = 0;
    virtual void setColor(const Vector& newColors, const float newShiness) = 0;
};

// Plane and Sphere declarations remain unchanged
class Plane : public Object {
public:
    Vector normal;
    Vector colors;
    float d;
    float shininess;
    bool reflective;
    bool transparent;
    virtual ~Plane(); 
    Plane(const Vector& n, float dist, const Vector& color, float s, bool t, bool r);
    Intersection intersect(const Ray& ray) override;
    void setColor(const Vector& newColors, const float newShiness) override;
    Vector checkerboardColor(const Vector& baseColor, const Vector& hitPoint)  ;
};

class Sphere : public Object {
public:
    Vector center;
    float radius;
    Vector colors;
    float shininess;
    bool reflective;
    bool transparent;
    virtual ~ Sphere();
    Sphere(const Vector& c, float radius, const Vector& color, float s, bool t, bool reflective);
    Intersection intersect(const Ray& ray)  override;
    void setColor(const Vector& newColors, const float newShiness) override;
};

class Cylinder : public Object {
public:
    Vector center;       // Center of the cylinder (middle of its height)
    Vector axis;         // Unit vector along the cylinder's axis
    float radius;        // Cylinder radius
    float height;        // Cylinder height
    Vector colors;       // Material color
    float shininess;     // Material shininess
    bool reflective;     // Reflectivity flag
    bool transparent;    // Transparency flag

    virtual ~Cylinder();
    Cylinder(const Vector& center, const Vector& axis, float radius, float height, const Vector& colors, float shininess, bool reflective, bool transparent);
    Intersection intersect(const Ray& ray)  override;
    void setColor(const Vector& newColors, const float newShiness) override;
};

#endif // OBJECT_H
