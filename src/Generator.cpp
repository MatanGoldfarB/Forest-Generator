#include "Vector.h"
#include "Scene.h"
#include "Object.h"
#include "Intersection.h"
#include "Light.h"
#include <cmath>
#include <fstream>
#include <iostream>

//find the closest object
Intersection findObject(const Ray& ray, const Scene& scene) {
    Intersection closestHit;
    for (const auto& obj : scene.objects) {
        Intersection tempHit = obj->intersect(ray);
        if (tempHit.hit && tempHit.distance < closestHit.distance) {
            closestHit = tempHit;
        }
    }
    return closestHit;
}

//find the ligth that that effect the object
std::vector<Light*> findLights(const Scene& scene, const Intersection& interObject) {
    std::vector<Light*> lightsForHitPoint;
    for (const auto& light : scene.lights) {
        if (DirectionalLight* directionalLight = dynamic_cast<DirectionalLight*>(light)) {
            Vector shadowRayDirection = (directionalLight->getDirection() * -1).normalize();
            Ray shadowRay(interObject.point + shadowRayDirection * 1e-4f, shadowRayDirection);

            bool inShadow = false;
            for (const auto& obj : scene.objects) {
                Intersection shadowIntersection = obj->intersect(shadowRay);
                if (shadowIntersection.hit) {
                    inShadow = true;
                    break;
                }
            }
            if (!inShadow) {
                lightsForHitPoint.push_back(directionalLight);
            }
        } else if (Spotlight* spotlight = dynamic_cast<Spotlight*>(light)) {
            Vector lightToPoint = (interObject.point - spotlight->position).normalize();
            float cosAngle = lightToPoint.dot(spotlight->getDirection().normalize());
            if (cosAngle >= spotlight->cutoffAngle) {
                Vector shadowRayDirection = (spotlight->position - interObject.point).normalize();
                Ray shadowRay(interObject.point + shadowRayDirection * 1e-4f, shadowRayDirection);

                bool inShadow = false;
                for (const auto& obj : scene.objects) {
                    Intersection shadowIntersection = obj->intersect(shadowRay);
                    if (shadowIntersection.hit) {
                        float lightDistance = (spotlight->position - interObject.point).magnitude();
                        if (shadowIntersection.distance < lightDistance) {
                            inShadow = true;
                            break;
                        }
                    }
                }
                if (!inShadow) {
                    lightsForHitPoint.push_back(spotlight);
                }
            }
        }
    }
    return lightsForHitPoint;
}

//calculating alpha and theta from the class 
float calcTheta(const Vector& normal, const Vector& lightDir) {
    return std::max(0.0f, std::abs(normal.normalize().dot(lightDir.normalize())));
}
float calcAlpha(const Vector& normal, const Vector& lightDir, const Vector& viewDir) {
    Vector reflectionDir = (normal * 2.0f * lightDir.dot(normal) - lightDir).normalize();
    return std::max(0.0f, viewDir.dot(reflectionDir));
}

// calculate the reflrct direction from the class
Vector reflect(const Vector& I, const Vector& N) {
    return I - N * 2.0f * I.dot(N);
}

//calculate the reflrct direction of the transperent object
Vector refract(const Vector& I, const Vector& N, float eta) {
    float cosi = std::clamp(I.dot(N), -1.0f, 1.0f);
    float etai = 1.0f, etat = eta;
    Vector n = N;
    if (cosi < 0) cosi = -cosi; 
    else { std::swap(etai, etat); n = N * -1.0f; }
    float etaRatio = etai / etat;
    float k = 1 - etaRatio * etaRatio * (1 - cosi * cosi);
    return k < 0 ? Vector(0, 0, 0) : I * etaRatio + n * (etaRatio * cosi - std::sqrt(k));
}

//calculate the pixels color
Vector createColor(Ray ray, Scene& scene, int counter) {
    if (counter > 5) return Vector(0, 0, 0);

    Intersection interObject = findObject(ray, scene);
    if (!interObject.hit) return Vector(0, 0, 0);

    Vector finalColor(0, 0, 0);
    Vector viewDir = (ray.origin - interObject.point).normalize();
    //recursive reflect object
    if (interObject.reflective) {
        Vector reflectedDir = reflect(ray.direction, interObject.normal).normalize();
        Ray reflectedRay(interObject.point + reflectedDir * 1e-4f, reflectedDir);
        finalColor = finalColor + createColor(reflectedRay, scene, counter + 1);
        return finalColor;
    }
    //recursive transparent object
    if (interObject.transparent) {
        float refractiveIndex = 1.5f;
        Vector refractedDir = refract(ray.direction, interObject.normal, refractiveIndex).normalize();
        Ray refractedRay(interObject.point + refractedDir * 1e-4f, refractedDir);
        finalColor = finalColor + createColor(refractedRay, scene, counter + 1);
        return finalColor;
    }
    //for transparent reflect the I vector will be (0,0,0)
    for (const auto& light : findLights(scene, interObject)) {
        float cosTheta = calcTheta(interObject.normal, light->getDistance(interObject.point));
        float cosAlpha = calcAlpha(interObject.normal, light->getDistance(interObject.point), viewDir);
        float ncosAlpha = pow(cosAlpha, interObject.shininess);
        Vector diffuse = interObject.color * cosTheta;
        Vector specular = Vector(0.7, 0.7, 0.7) * ncosAlpha;
        Vector I = diffuse + specular;
        finalColor = finalColor + I.Hadamard(light->getIntensity());
    }

    finalColor = finalColor + interObject.color.Hadamard(scene.ambientLight->getIntensity());
    return finalColor;
}

void saveImage(int width, int height, const std::vector<Vector>& buffer, const std::string& fileName) {
    std::ofstream file(fileName, std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to save the image to " << fileName << std::endl;
        return;
    }
    file << "P6\n" << width << " " << height << "\n255\n";
    for (const auto& color : buffer) {
        file.put(static_cast<unsigned char>(std::min(1.0f, std::max(0.0f, color.x)) * 255));
        file.put(static_cast<unsigned char>(std::min(1.0f, std::max(0.0f, color.y)) * 255));
        file.put(static_cast<unsigned char>(std::min(1.0f, std::max(0.0f, color.z)) * 255));
    }
    file.close();
    std::cout << "Image saved to " << fileName << std::endl;
}

#include <cstdlib> // For std::rand and std::srand
#include <ctime>   // For seeding random number generator

//creating and sending the rays
void renderImage(int imageWidth, int imageHeight, const std::string& dataPath, Scene& scene) {
    float screenWidth = 2.0f, screenHeight = 2.0f;
    scene.loadFromFile(dataPath);
    int raysPerPixel = 1;
    // Extract the input file name
    std::filesystem::path inputPath(dataPath);
    std::string inputFileName = inputPath.stem().string();
    std::string outputFileName = "outputs/my" + inputFileName + ".png";
   
    //if we want more then one ray, will change the ray nomber and the output name
    if (scene.aliasing) {
        outputFileName = "outputs/myAliasing" + inputFileName + ".png";
        raysPerPixel = 10;
    }

    float pixelWidth = screenWidth / imageWidth;
    float pixelHeight = screenHeight / imageHeight;
    std::vector<Vector> imageBuffer(imageWidth * imageHeight);

   int subGridX = std::ceil(std::sqrt(raysPerPixel));
    int subGridY = std::ceil(static_cast<float>(raysPerPixel) / subGridX);

    for (int j = 0; j < imageHeight; j++) {
        for (int i = 0; i < imageWidth; i++) {
            Vector accumulatedColor(0, 0, 0);

            for (int sx = 0; sx < subGridX; ++sx) {
                for (int sy = 0; sy < subGridY; ++sy) {
                    if (sx * subGridY + sy >= raysPerPixel) {
                        continue; // Skip extra sub-pixels if raysPerPixel is not perfectly divisible
                    }

                    // Compute sub-pixel position
                    float subPixelX = -1.0f + (i + (sx + 0.5f) / subGridX) * pixelWidth;
                    float subPixelY = -1.0f + (j + (sy + 0.5f) / subGridY) * pixelHeight;

                    Vector subPixelPosition(subPixelX, subPixelY, 0);
                    Vector rayDirection = (subPixelPosition - scene.cameraPosition).normalize();
                    Ray ray(scene.cameraPosition, rayDirection);

                    // Accumulate color from this ray
                    accumulatedColor = accumulatedColor + createColor(ray, scene, 0);
                }
            }
            accumulatedColor = accumulatedColor / float(raysPerPixel);

            // Store the final color in the image buffer
            int idx = (imageHeight - j - 1) * imageWidth + i;
            imageBuffer[idx] = accumulatedColor;
        }
    }

    // Save the image
    saveImage(imageWidth, imageHeight, imageBuffer, outputFileName);
}




#include <iostream>

#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./racetracer <scene file path>" << std::endl;
        return 1;
    }

    std::string dataPath = argv[1];
    int imageWidth = 800;
    int imageHeight = 800;

    
    // Print the paths for debugging
    std::cout << "Input file: " << dataPath << std::endl;

    Scene scene;
    renderImage(imageWidth, imageHeight, dataPath, scene);

    return 0;
}


