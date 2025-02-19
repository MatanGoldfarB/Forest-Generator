#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <limits>
#include "Vector.h"

// Vector utility class

    Vector :: Vector() : x(0), y(0), z(0) {}
    Vector :: Vector(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector Vector :: operator+(const Vector& other) const {
        return Vector(x + other.x, y + other.y, z + other.z);
    }

    Vector Vector :: operator-() const {
        return Vector(-x, -y, -z);
    }

    Vector Vector :: operator-(const Vector& other) const {
        return Vector(x - other.x, y - other.y, z - other.z);
    }

    Vector Vector :: operator*(float scalar) const {
        return Vector(x * scalar, y * scalar, z * scalar);
    }

    Vector Vector :: operator/(float scalar) const {
        return Vector(x / scalar, y / scalar, z / scalar);
    }

    float Vector :: dot(const Vector& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector Vector :: cross(const Vector& other) const {
        return Vector(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    Vector Vector :: Hadamard(const Vector& other) const{
        return Vector(
            x * other.x,
            y * other.y,
            z * other.z );
    }

    Vector Vector :: normalize() const {
        float length = std::sqrt(x * x + y * y + z * z);
        return Vector(x / length, y / length, z / length);
    }
    float Vector :: magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float Vector :: getZ() const {
        return z;
    }
    Vector& Vector :: operator+=(const Vector& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }






