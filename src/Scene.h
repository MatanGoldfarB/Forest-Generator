#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include "Vector.h"

class Object;
class Light;
class AmbientLight;

class Scene {
public:
    Scene();
    ~Scene();

    void loadFromFile(const std::string& filename);

    Vector cameraPosition;
    bool aliasing;
    AmbientLight* ambientLight = nullptr;
    std::vector<Object*> objects;
    std::vector<Light*> lights;


private:
    int objCounter;
    int lightCounter;
    int pointCounter;
};

#endif
