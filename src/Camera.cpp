#include "Camera.h"

Camera::Camera(const Vector& pos, const Vector& fw, const Vector& _up, 
    float fLength, float sWidth, float sHeight, 
    int iWidth, int iHeight)
    : position(pos),
    forward(fw),
    up(_up),
    focalLength(fLength),
    sensorWidth(sWidth),
    sensorHeight(sHeight),
    imageWidth(iWidth),
    imageHeight(iHeight)
{
    right = forward.cross(up).normalize();
}

Ray Camera::generateRay(const int x, const int y) const
{
    float pixelUp = -sensorHeight * 0.5f + x*(sensorHeight/imageHeight);
    float pixelRight = -sensorWidth * 0.5f + y*(sensorWidth/imageWidth);

    Vector direction = (forward * focalLength + up * pixelUp + right * pixelRight).normalize();
    return Ray(position, direction);
}