#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <limits>
#include "Object.h"
#include "Vector.h"
#include "Scene.h"
#include "Intersection.h"


Object :: ~Object() {}

//plane
Plane :: ~Plane() {}
Plane::Plane(const Vector& n, float dist, const Vector& color, float s, bool t, bool r)
    : colors(color), shininess(s), reflective(r), transparent(t) {
    
    float magnitude = n.magnitude();
    if (magnitude > 1e-6) {
        normal = n / magnitude; // Normalize the normal vector
        d = dist / magnitude;   // Adjust 
    } else {
        // Handle the edge case where the normal vector's magnitude is zero
        normal = Vector(0, 0, 0);
        d = 0;
        std::cerr << "Warning: Plane normal vector magnitude is zero. Plane may be invalid." << std::endl;
    }
}
Intersection Plane::intersect(const Ray& ray) {
    float denominator = normal.dot(ray.direction);

    // Check if the ray is parallel to the plane
    if (std::abs(denominator) < 1e-6) {
        return Intersection(); 
    }
    float t = -(normal.dot(ray.origin) + d) / denominator;
    // Check if the intersection is behind the ray's origin
    if (t < 1e-6) {
        return Intersection();
    }

    Vector intersectionPoint = ray.origin + ray.direction * t;
    Vector baseColor = colors;
    Vector color = checkerboardColor(baseColor, intersectionPoint);
    return Intersection(true, t, intersectionPoint, normal.normalize(), color, shininess, reflective, transparent);
}


void Plane :: setColor(const Vector& newColors, const float newShiness) {
    if (!reflective && !transparent )
    {
        colors = newColors;
        shininess = newShiness;
    }
    
}

Vector Plane :: checkerboardColor(const Vector& baseColor, const Vector& hitPoint)  {
    float scaleParameter = 0.5f;
    float checkerboard = 0;

    if (hitPoint.x < 0) {
        checkerboard += std::floor((0.5f - hitPoint.x) / scaleParameter);
    } else {
        checkerboard += std::floor(hitPoint.x / scaleParameter);
    }
    if (hitPoint.y < 0) {
        checkerboard += std::floor((0.5f - hitPoint.y) / scaleParameter);
    } else {
        checkerboard += std::floor(hitPoint.y / scaleParameter);
    }

    checkerboard = (checkerboard * 0.5f) - static_cast<int>(checkerboard * 0.5f);
    checkerboard *= 2;

    if (checkerboard > 0.5f) {
        return baseColor * 0.5f; 
    }
    return baseColor; 
    }



// Sphere 
Sphere :: ~Sphere(){}

Sphere :: Sphere(const Vector& c, float radius, const Vector& color, float s, bool t, bool reflective)
    : center(c), radius(radius), colors(color), shininess(s), reflective(reflective), transparent(t) {}

Intersection Sphere::intersect(const Ray& ray) {
    Vector rayOrigin = ray.origin; 
    Vector rayDirection = ray.direction; 
    Vector sphereCenter = center; 
    Vector toCenter = sphereCenter - rayOrigin; 

    float projectionLength = toCenter.dot(rayDirection); 
    float perpendicularDist2 = toCenter.dot(toCenter) - projectionLength * projectionLength; 

    if (perpendicularDist2 > radius * radius) {
        return Intersection(); 
    }

    float halfChord = std::sqrt(radius * radius - perpendicularDist2); 
    float t0 = projectionLength - halfChord;
    float t1 = projectionLength + halfChord;

    // Find the closest positive t
    float t = std::numeric_limits<float>::infinity();
    if (t0 > 1e-6 && t1 > 1e-6) {
        t = std::min(t0, t1);
    } else if (t1 > 1e-6) {
        t = t1;
    } else if (t0 > 1e-6) {
        t = t0;
    }

    if (t == std::numeric_limits<float>::infinity()) {
        return Intersection(); 
    }

    Vector intersectionPoint = rayOrigin + rayDirection * t; 
    Vector normal = (intersectionPoint - sphereCenter).normalize(); 

    return Intersection(true, t, intersectionPoint, normal, colors, shininess, reflective, transparent);
}


void Sphere :: setColor(const Vector& newColors, const float newShiness) {
    if (!reflective && !transparent )
    {
        colors = newColors;
        shininess = newShiness;
    }
    
}

// Sphere 
Cylinder :: ~Cylinder(){}

Cylinder :: Cylinder(const Vector& center, const Vector& axis, float radius, float height, const Vector& colors, float shininess, bool reflective, bool transparent)
    : center(center), radius(radius), height(height), colors(colors), shininess(shininess), reflective(reflective), transparent(transparent) {
    // Ensure the axis is normalized
    this->axis = axis.normalize();
}

Intersection Cylinder::intersect(const Ray& ray) {
    // Extract ray origin and direction.
    Vector O = ray.origin;
    Vector d = ray.direction; // Assumed normalized.
    
    // For clarity, denote:
    //   C = cylinder center, v = cylinder axis.
    Vector C = center;
    Vector v = axis;  // Already normalized.
    
    // Compute the vector from the cylinder's center to the ray origin.
    Vector CO = O - C;

    // ----- 1. Intersect with the infinite cylinder (curved surface) -----
    // Decompose the ray direction into components parallel and perpendicular to the cylinder's axis.
    float d_dot_v = d.dot(v);
    Vector d_parallel = v * d_dot_v;
    Vector d_perp = d - d_parallel;

    // Similarly decompose the CO vector.
    float CO_dot_v = CO.dot(v);
    Vector CO_parallel = v * CO_dot_v;
    Vector CO_perp = CO - CO_parallel;

    // Solve quadratic: A t^2 + B t + C_coef = 0,
    // where A = |d_perp|^2, B = 2*(d_perp · CO_perp), and
    // C_coef = |CO_perp|^2 - radius^2.
    float A = d_perp.dot(d_perp);
    float B = 2.0f * d_perp.dot(CO_perp);
    float C_coef = CO_perp.dot(CO_perp) - radius * radius;

    float tCylinder = std::numeric_limits<float>::infinity();
    bool hitSide = false;

    // Only solve the quadratic if A is not (almost) zero.
    if (std::abs(A) > 1e-6f) {
        float discriminant = B * B - 4 * A * C_coef;
        if (discriminant >= 0.0f) {
            float sqrtDisc = std::sqrt(discriminant);
            float t0 = (-B - sqrtDisc) / (2 * A);
            float t1 = (-B + sqrtDisc) / (2 * A);

            // Check each solution to see if it lies within the finite cylinder's height.
            if (t0 > 1e-6f) {
                Vector P0 = O + d * t0;
                float y0 = (P0 - C).dot(v);
                if (std::abs(y0) <= height / 2.0f) {
                    tCylinder = t0;
                    hitSide = true;
                }
            }
            if (t1 > 1e-6f) {
                Vector P1 = O + d * t1;
                float y1 = (P1 - C).dot(v);
                if (std::abs(y1) <= height / 2.0f) {
                    // Use the smaller positive t that lies within the cylinder.
                    if (t1 < tCylinder) {
                        tCylinder = t1;
                        hitSide = true;
                    }
                }
            }
        }
    }

    // ----- 2. Intersect with the cylinder caps -----
    float tCap = std::numeric_limits<float>::infinity();
    bool hitCap = false;
    Vector capNormal;  // Normal of the cap that is hit.

    // Define the centers of the top and bottom caps.
    Vector capCenterTop = C + v * (height / 2.0f);
    Vector capCenterBottom = C - v * (height / 2.0f);

    // Check top cap:
    {
        // The plane for the top cap: (P - capCenterTop)·v = 0.
        float denom = d.dot(v);
        if (std::abs(denom) > 1e-6f) {
            float tTop = (capCenterTop - O).dot(v) / denom;
            if (tTop > 1e-6f) {
                Vector PTop = O + d * tTop;
                // Check if PTop is inside the disk (radius check).
                Vector diff = PTop - capCenterTop;
                if (diff.dot(diff) <= radius * radius) {
                    tCap = std::min(tCap, tTop);
                    hitCap = true;
                    capNormal = v;  // Normal for the top cap.
                }
            }
        }
    }

    // Check bottom cap:
    {
        float denom = d.dot(v);
        if (std::abs(denom) > 1e-6f) {
            float tBottom = (capCenterBottom - O).dot(v) / denom;
            if (tBottom > 1e-6f) {
                Vector PBottom = O + d * tBottom;
                Vector diff = PBottom - capCenterBottom;
                if (diff.dot(diff) <= radius * radius) {
                    if (tBottom < tCap) { // Choose the closer cap hit.
                        tCap = tBottom;
                        hitCap = true;
                        capNormal = -v;  // Normal for the bottom cap.
                    }
                }
            }
        }
    }

    // ----- 3. Choose the closest valid intersection -----
    float t = std::numeric_limits<float>::infinity();
    bool sideHit = false; // Flag to remember which surface was hit.

    if (hitSide && tCylinder < t) {
        t = tCylinder;
        sideHit = true;
    }
    if (hitCap && tCap < t) {
        t = tCap;
        sideHit = false;
    }

    // If no valid intersection was found, return an empty Intersection.
    if (t == std::numeric_limits<float>::infinity()) {
        return Intersection();
    }

    // Compute the intersection point.
    Vector intersectionPoint = O + d * t;

    // Compute the surface normal.
    Vector normal;
    if (sideHit) {
        // For the curved surface, project (P - C) onto the plane perpendicular to the axis.
        Vector temp = intersectionPoint - C;
        normal = (temp - v * temp.dot(v)).normalize();
    } else {
        // For a cap, use the precomputed cap normal.
        normal = capNormal;
    }

    // Return the hit. (The parameters passed to Intersection match those of your sphere.)
    return Intersection(true, t, intersectionPoint, normal, colors, shininess, reflective, transparent);
}


void Cylinder :: setColor(const Vector& newColors, const float newShiness) {
    if (!reflective && !transparent )
    {
        colors = newColors;
        shininess = newShiness;
    }
    
}




