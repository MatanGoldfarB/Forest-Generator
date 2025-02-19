#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Scene.h"
#include "Object.h"
#include "Light.h"
#include "Intersection.h"
#include "Vector.h"
#include <cmath>

Scene::Scene() 
    : cameraPosition(0, 0, 0),
      aliasing(false),
      ambientLight(nullptr),
      objCounter(0),
      lightCounter(0),
      pointCounter(0)
{}

Scene::~Scene() {
    for (Object* obj : objects) {
        delete obj;
    }
    for (Light* light : lights) {
        delete light;
    }
}

void Scene::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open scene file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if(line.empty()) continue;
        std::istringstream iss(line);
        char type;
        iss >> type;
        switch (type) {
            case 'e': {
                float x, y, z, a;
                iss >> x >> y >> z >> a;
                cameraPosition = Vector(x, y, z);
                if (a - 0.0 < 1e-6){
                    aliasing = true;
                }
                break;
            }
            case 'a': {
                float r, g, b;
                iss >> r >> g >> b;
                ambientLight = new AmbientLight(Vector(r, g, b));
                break;
            }
            case 'o': {
                // Non-transparent, non-reflective objects
                float x, y, z, param;
                iss >> x >> y >> z >> param;
                if (param > 0) {
                    // Sphere
                    Vector center(x, y, z);
                    objects.push_back(new Sphere(center, param, Vector(0, 0, 0), 0, false, false));
                } else {
                    // Plane
                    Vector normal(x, y, z);
                    objects.push_back(new Plane(normal, param, Vector(0, 0, 0), 0, false, false));
                }
                break;
            }
            case 't': {
                // Transparent objects
                float x, y, z, param;
                iss >> x >> y >> z >> param;
                if (param > 0) {
                    Vector center(x, y, z);
                    objects.push_back(new Sphere(center, param, Vector(0, 0, 0), 0, true, false));
                } else {
                    Vector normal(x, y, z);
                    objects.push_back(new Plane(normal, param, Vector(0, 0, 0), 0, true, false));
                }
                break;
            }
            case 'r': {
                // Reflective objects
                float x, y, z, param;
                iss >> x >> y >> z >> param;
                if (param > 0) {
                    Vector center(x, y, z);
                    objects.push_back(new Sphere(center, param, Vector(0, 0, 0), 0, false, true));
                } else {
                    Vector normal(x, y, z);
                    objects.push_back(new Plane(normal, param, Vector(0, 0, 0), 0, false, true));
                }
                break;
            }
            case 'd': {
                float x, y, z, l;
                iss >> x >> y >> z >> l;
                Vector direction(x, y, z);
                if (l == 0.0f) {
                    // Directional Light
                    Vector intensity = (ambientLight ? ambientLight->getIntensity() : Vector(0,0,0));
                    lights.push_back(new DirectionalLight(direction, intensity));
                } else {
                    // Spotlight with default position and cutoff, will be set later
                    Vector intensity = (ambientLight ? ambientLight->getIntensity() : Vector(0,0,0));
                    lights.push_back(new Spotlight(Vector(0,0,0), direction, 0, intensity));
                }
                break;
            }
            case 'p': {
                float px, py, pz, cutoff;
                iss >> px >> py >> pz >> cutoff;
                if (pointCounter < (int)lights.size()) {
                    Spotlight* spotLight = dynamic_cast<Spotlight*>(lights[pointCounter]);
                    if (spotLight) {
                        spotLight->setPosition(Vector(px, py, pz), cutoff);
                        pointCounter++;
                    } else {
                        std::cerr << "Error: Attempted to set position for a non-spotlight light." << std::endl;
                    }
                } else {
                    std::cerr << "Error: Mismatch in spotlight position assignment. Not enough spotlights." << std::endl;
                }
                break;
            }
            case 'c': {
                float r, g, b, shininess;
                iss >> r >> g >> b >> shininess;
                if (objCounter < (int)objects.size()) {
                    objects[objCounter]->setColor(Vector(r, g, b), shininess);
                    objCounter++;
                } else {
                    std::cerr << "Error: Mismatch in object color assignment. More 'c' lines than objects." << std::endl;
                }
                break;
            }
            case 'i': {
                float r, g, b;
                iss >> r >> g >> b;
                if (lightCounter < (int)lights.size()) {
                    lights[lightCounter]->setIntensity(Vector(r, g, b));
                    lightCounter++;
                } else {
                    std::cerr << "Error: Mismatch in light intensity assignment. More 'i' lines than lights." << std::endl;
                }
                break;
            }
            case 'f': {
                // Cylinder object
                float cx, cy, cz, ax, ay, az, rad, h;
                iss >> cx >> cy >> cz >> ax >> ay >> az >> rad >> h;
                Vector center(cx, cy, cz);
                Vector axis(ax, ay, az);
                objects.push_back(new Cylinder(center, axis, rad, h, Vector(0, 0, 0), 0, false, false));
                break;
            }
            default: {
                std::cerr << "Unknown line type: " << type << " in scene file." << std::endl;
                break;
            }
        }
    }

    file.close();

}
