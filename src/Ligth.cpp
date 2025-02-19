#include <limits>
#include "Light.h"


// Base Light class

    Light:: ~Light() {}



// Ambient Light class

    AmbientLight :: AmbientLight(const Vector& inten) : intensity(inten) {}

    Vector AmbientLight :: getDirection() const  {
        return Vector(0, 0, 0);
    }

    Vector AmbientLight :: getIntensity() const  {
        return intensity;
    }

    void AmbientLight :: setPosition(Vector /*pos*/, float /*cutoff*/)  {}

    Vector AmbientLight :: getDistance(Vector& /*pos*/) const  {
        return  Vector(0, 0, 0);
    }
    void AmbientLight :: setIntensity(Vector color) {
        intensity = color;
    }



// Directional Light class

    DirectionalLight :: DirectionalLight(const Vector& dir, const Vector& inten) : direction(dir), intensity(inten) {}

    Vector DirectionalLight :: getDirection() const  {
        return direction;
    }

    Vector DirectionalLight :: getIntensity() const  {
        return intensity;
    }

    void DirectionalLight :: setPosition(Vector /*pos*/, float /*cutoff*/)  {}

    Vector DirectionalLight :: getDistance(Vector& /*pos*/) const  {
        return  direction*-1;
    }

    void DirectionalLight :: setIntensity(Vector color) {
        intensity = color;
    }

// Spotlight class

    Spotlight :: Spotlight(const Vector& pos, const Vector& dir, float cutoff, const Vector& inten)
        : position(pos), direction(dir), cutoffAngle(cutoff), intensity(inten) {}

    Vector Spotlight :: getDirection() const  {
        return direction;
    }

    Vector Spotlight :: getIntensity() const  {
        return intensity;
    }

    void Spotlight :: setPosition(Vector pos, float cutoff)  {
        position = pos;
        cutoffAngle = cutoff;
    }

    Vector Spotlight :: getDistance( Vector& point) const  {
        return (position - point).normalize();
    }
    
        void Spotlight :: setIntensity(Vector color) {
        intensity = color;
    }


