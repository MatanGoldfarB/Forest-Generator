#pragma once
#include "Vector.h"
#include "Ray.h"

class Camera {
    private:
        Vector position;
        Vector forward;
        Vector up;
        Vector right;
        float focalLength;
        float sensorWidth;
        float sensorHeight;
        int imageWidth;
        int imageHeight;
        
    public:
        Camera(const Vector& pos, const Vector& fw, const Vector& _up, 
            float fLength, float sWidth, float sHeight, int iWidth, int iHeight);
        Ray generateRay(const int x, const int y) const;
};