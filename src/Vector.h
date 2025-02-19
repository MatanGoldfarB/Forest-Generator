#ifndef VECTOR_H
#define VECTOR_H

#include <limits>
class Vector {
public:
    float x, y, z;

    Vector() ;
    Vector(float x, float y, float z) ;

    Vector operator+(const Vector& other) const;
    Vector operator-() const;
    Vector operator-(const Vector& other) const;
    Vector operator*(float scalar) const;
    Vector operator/(float scalar) const;

    float dot(const Vector& other) const;
    Vector cross(const Vector& other) const;
    Vector normalize() const;
    float magnitude() const;
    float getZ() const ;
    Vector& operator+=(const Vector& other) ;
    Vector Hadamard(const Vector& other) const;

};

#endif // VECTOR_H
