
// Light.h
#ifndef LIGHT_H
#define LIGHT_H

#include "Vector.h"

class Light {
public:
    virtual ~Light();
    virtual Vector getDirection() const = 0;
    virtual Vector getIntensity() const = 0;
    virtual void setPosition(Vector pos, float cutoff) = 0;
    virtual Vector getDistance( Vector& point) const = 0 ;
    virtual void setIntensity(Vector color) = 0;
};

class AmbientLight : public Light {
public:
    Vector intensity;

    AmbientLight(const Vector& inten) ;
    Vector getDirection() const override;
    Vector getIntensity() const override;
    void setPosition(Vector /*pos*/, float /*cutoff*/) override ;
    Vector getDistance(Vector& /*pos*/) const override;
    void setIntensity(Vector color) override;
};

class DirectionalLight : public Light {
public:
    Vector direction;
    Vector intensity;

    DirectionalLight(const Vector& dir, const Vector& inten) ;
    Vector getDirection() const override;
    Vector getIntensity() const override;
    void setPosition(Vector /*pos*/, float /*cutoff*/) override ;
    Vector getDistance(Vector& /*pos*/) const override;
    void setIntensity(Vector color) override;
};

class Spotlight : public Light {
public:
    Vector position;
    Vector direction;
    float cutoffAngle;
    Vector intensity;

    Spotlight(const Vector& pos, const Vector& dir, float cutoff, const Vector& inten);

    Vector getDirection() const override;
    Vector getIntensity() const override;
    void setPosition(Vector /*pos*/, float /*cutoff*/) override ;
    Vector getDistance(Vector& /*pos*/) const override;
    void setIntensity(Vector color) override;

};

#endif // LIGHT_H