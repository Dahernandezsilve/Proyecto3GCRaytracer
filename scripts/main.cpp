#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <cstdlib>
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/geometric.hpp"
#include <string>
#include "glm/glm.hpp"
#include <vector>
#include "print.h"
#include "skybox.h"
#include "color.h"
#include "intersect.h"
#include "object.h"
#include "sphere.h"
#include "cube.h"
#include "light.h"
#include "camera.h"
#include "glm/ext/matrix_transform.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
const int MAX_RECURSION = 3;
const float BIAS = 0.0001f;

SDL_Renderer* renderer;
std::vector<Object*> objects;
Light light(glm::vec3(-20.0, -20, 20), 1.5f, Color(255, 255, 255));
Camera camera(glm::vec3(0.0, 0.0, 15.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);
Skybox skybox("../textures/minecraft.jpg");

void point(glm::vec2 position, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, position.x, position.y);
}

float castShadow(const glm::vec3& shadowOrigin, const glm::vec3& lightDir, Object* hitObject) {
    for (auto& obj : objects) {
        if (obj != hitObject) {
            Intersect shadowIntersect = obj->rayIntersect(shadowOrigin, lightDir);
            if (shadowIntersect.isIntersecting && shadowIntersect.dist > 0) {
                float shadowRatio = shadowIntersect.dist / glm::length(light.position - shadowOrigin);
                shadowRatio = glm::min(1.0f, shadowRatio);
                return 1.0f - shadowRatio;
            }
        }
    }
    return 1.0f;
}

Color castRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const short recursion = 0) {
    float zBuffer = 99999;
    Object* hitObject = nullptr;
    Intersect intersect;

    for (const auto& object : objects) {
        Intersect i;
        if (dynamic_cast<Cube*>(object) != nullptr) {
            i = dynamic_cast<Cube*>(object)->rayIntersect(rayOrigin, rayDirection);
        } else if (dynamic_cast<Sphere*>(object) != nullptr) {
            i = dynamic_cast<Sphere*>(object)->rayIntersect(rayOrigin, rayDirection);
        }
        if (i.isIntersecting && i.dist < zBuffer) {
            zBuffer = i.dist;
            hitObject = object;
            intersect = i;
        }
    }

    if (!intersect.isIntersecting || recursion == MAX_RECURSION) {
        return skybox.getColor(rayDirection);  // Sky color
    }


    glm::vec3 lightDir = glm::normalize(light.position - intersect.point);
    glm::vec3 viewDir = glm::normalize(rayOrigin - intersect.point);
    glm::vec3 reflectDir = glm::reflect(-lightDir, intersect.normal);

    float shadowIntensity = castShadow(intersect.point, lightDir, hitObject);

    float diffuseLightIntensity = std::max(0.0f, glm::dot(intersect.normal, lightDir));
    float specReflection = glm::dot(viewDir, reflectDir);

    Material mat = hitObject->material;

    float specLightIntensity = std::pow(std::max(0.0f, glm::dot(viewDir, reflectDir)), mat.specularCoefficient);

    Color reflectedColor(0.0f, 0.0f, 0.0f);
    if (mat.reflectivity > 0) {
        glm::vec3 origin = intersect.point + intersect.normal * BIAS;
        reflectedColor = castRay(origin, reflectDir, recursion + 1);
    }

    Color refractedColor(0.0f, 0.0f, 0.0f);
    if (mat.transparency > 0) {
        glm::vec3 origin = intersect.point - intersect.normal * BIAS;
        glm::vec3 refractDir = glm::refract(rayDirection, intersect.normal, mat.refractionIndex);
        refractedColor = castRay(origin, refractDir, recursion + 1);
    }

    Color diffuseLight = mat.diffuse * light.intensity * diffuseLightIntensity * mat.albedo * shadowIntensity;
    Color specularLight = light.color * light.intensity * specLightIntensity * mat.specularAlbedo * shadowIntensity;
    Color color = (diffuseLight + specularLight) * (1.0f - mat.reflectivity - mat.transparency) + reflectedColor * mat.reflectivity + refractedColor * mat.transparency;
    return color;
}

void setUp() {
    Material rubber = {
            Color(155,155,155),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material graySecond = {
            Color(145,145,145),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material grayThird = {
            Color(135,135,135),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material grayFourth = {
            Color(125,125,125),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material carbon = {
            Color(10,10,10),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material white = {
            Color(170,170,170),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material red = {
            Color(255,0,0),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };


    Material brown = {
            Color(108,94,83),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material brownWhite = {
            Color(140, 130, 120),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material brownSecond = {
            Color(103,82,65),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material brownThird = {
            Color(113,92,75),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material ivory = {
            Color(100, 100, 80),
            0.5,
            0.5,
            50.0f,
            0.4f,
            0.0f
    };

    Material mirror = {
            Color(255, 255, 255),
            0.0f,
            10.0f,
            1425.0f,
            0.9f,
            0.0f
    };

    Material glass = {
            Color(255, 255, 255),
            0.0f,
            10.0f,
            1425.0f,
            0.2f,
            1.0f,
    };

    //Face
    objects.push_back(new Cube(glm::vec3(-1.0f, -1.0f, -0.5f), glm::vec3(1.0f, 1.0f, 0.4f), rubber));
    //----BeardRight
    //--piramid
    objects.push_back(new Cube(glm::vec3(0.7f, -1.001f, -0.2f), glm::vec3(1.001f, -0.7f, 0.41f), brown));
    objects.push_back(new Cube(glm::vec3(0.7f, -0.7f, 0.1f), glm::vec3(1.001f, -0.4f, 0.4f), brown));objects.push_back(new Cube(glm::vec3(0.7f, -0.7f, 0.1f), glm::vec3(1.001f, -0.4f, 0.4f), brown));
    //--pixels
    objects.push_back(new Cube(glm::vec3(0.7f, -0.7f, 0.1f), glm::vec3(1.0f, -0.4f, 0.41f), brownSecond));
    objects.push_back(new Cube(glm::vec3(0.7f, -0.4f, 0.1f), glm::vec3(1.0f, -0.1f, 0.41f), brownWhite));
    objects.push_back(new Cube(glm::vec3(0.4f, -0.7f, 0.1f), glm::vec3(0.7f, -0.4f, 0.41f), brownWhite));
    objects.push_back(new Cube(glm::vec3(0.4f, -0.4f, 0.1f), glm::vec3(0.7f, -0.1f, 0.41f), brownSecond));
    objects.push_back(new Cube(glm::vec3(0.4f, -1.0f, 0.1f), glm::vec3(0.7f, -0.7f, 0.41f), brownThird));

    //----BeardLeft
    //--piramid
    objects.push_back(new Cube(glm::vec3(-1.0001f, -1.001f, -0.2f), glm::vec3(-0.7f, -0.7f, 0.41f), brown));
    objects.push_back(new Cube(glm::vec3(-1.001f, -0.7f, 0.1f), glm::vec3(-0.7f, -0.4f, 0.4f), brown));objects.push_back(new Cube(glm::vec3(0.7f, -0.7f, 0.1f), glm::vec3(1.001f, -0.4f, 0.4f), brown));
    //--pixels
    objects.push_back(new Cube(glm::vec3(-1.0f, -0.7f, 0.1f), glm::vec3(-0.7f, -0.4f, 0.41f), brownSecond));
    objects.push_back(new Cube(glm::vec3(-1.0f, -0.4f, 0.1f), glm::vec3(-0.7f, -0.1f, 0.41f), brownWhite));
    objects.push_back(new Cube(glm::vec3(-0.7f, -0.7f, 0.1f), glm::vec3(-0.4f, -0.4f, 0.41f), brownWhite));
    objects.push_back(new Cube(glm::vec3(-0.7f, -0.4f, 0.1f), glm::vec3(-0.4f, -0.1f, 0.41f), brownSecond));
    objects.push_back(new Cube(glm::vec3(-0.7f, -1.0f, 0.1f), glm::vec3(-0.4f, -0.7f, 0.41f), brownThird));

    //--eyebrows right
    objects.push_back(new Cube(glm::vec3(0.7f, 0.7f, 0.1f), glm::vec3(1.0f, 1.0f, 0.401f), grayThird));
    objects.push_back(new Cube(glm::vec3(0.4f, 0.7f, 0.1f), glm::vec3(0.7f, 1.0f, 0.401f), graySecond));

    objects.push_back(new Cube(glm::vec3(0.7f, 0.4f, 0.1f), glm::vec3(1.0f, 0.7f, 0.401f), graySecond));
    objects.push_back(new Cube(glm::vec3(0.4f, 0.4f, 0.1f), glm::vec3(0.7f, 0.7f, 0.401f), grayThird));

    //--eyebrows left
    objects.push_back(new Cube(glm::vec3(-1.0f, 0.7f, 0.1f), glm::vec3(-0.7f, 1.0f, 0.401f), grayThird));
    objects.push_back(new Cube(glm::vec3(-0.7f, 0.7f, 0.1f), glm::vec3(-0.4f, 1.0f, 0.401f), graySecond));

    objects.push_back(new Cube(glm::vec3(-1.0f, 0.4f, 0.1f), glm::vec3(-0.7f, 0.7f, 0.401f), graySecond));
    objects.push_back(new Cube(glm::vec3(-0.7f, 0.4f, 0.1f), glm::vec3(-0.4f, 0.7f, 0.401f), grayThird));
    objects.push_back(new Cube(glm::vec3(-0.4f, 0.4f, 0.1f), glm::vec3(0.4f, 0.7f, 0.401f), graySecond));

    //Eyes
    //----Left
    objects.push_back(new Cube(glm::vec3(-0.6f, 0.0f, 0.4f), glm::vec3(-0.3f, 0.3f, 0.41f), carbon));
    objects.push_back(new Cube(glm::vec3(-0.9f, 0.0f, 0.4f), glm::vec3(-0.6f, 0.3f, 0.41f), white));
    //---Between
    objects.push_back(new Cube(glm::vec3(-0.3f, 0.0f, 0.4f), glm::vec3(0.3f, 0.3f, 0.41f), brown));
    //----Right
    objects.push_back(new Cube(glm::vec3(0.3f, 0.0f, 0.4f), glm::vec3(0.6f, 0.3f, 0.41f), carbon));
    objects.push_back(new Cube(glm::vec3(0.6f, 0.0f, 0.4f), glm::vec3(0.9f, 0.3f, 0.41f), white));

    //Snout
    objects.push_back(new Cube(glm::vec3(-0.45f, -1.0f, 0.4f), glm::vec3(0.45f, 0.0f, 1.3f), brownWhite));
    //----OnSnout
    objects.push_back(new Cube(glm::vec3(-0.45f, 0.0f, 0.4f), glm::vec3(0.45f, 0.001f, 1.3f), brown));
    //----line
    objects.push_back(new Cube(glm::vec3(-0.46f, -1.0f, 0.4f), glm::vec3(0.46f, -0.7f, 1.32f), carbon));
    //----nose
    objects.push_back(new Cube(glm::vec3(-0.15f, -0.29f, 1.01f), glm::vec3(0.15f, 0.01f, 1.31f), carbon));
    //----mustache
    objects.push_back(new Cube(glm::vec3(-0.46f, -0.7f, 0.4f), glm::vec3(0.46f, -0.4f, 1.0f), brown));
    objects.push_back(new Cube(glm::vec3(-0.46f, -0.4f, 0.4f), glm::vec3(0.46f, -0.1f, 0.7f), brown));

    //Ears
    objects.push_back(new Cube(glm::vec3(-1.0f, 1.0f, -0.5f), glm::vec3(-0.3f, 1.6f, -0.2f), rubber));
    objects.push_back(new Cube(glm::vec3(-1.01f, 1.0f, -0.47f), glm::vec3(-0.299f, 1.601f, -0.23f), carbon));

    objects.push_back(new Cube(glm::vec3(0.3f, 1.0f, -0.5f), glm::vec3(1.0f, 1.6f, -0.2f), rubber));
    objects.push_back(new Cube(glm::vec3(0.299f, 1.0f, -0.47f), glm::vec3(1.01f, 1.601f, -0.23f), carbon));

    //Body-front
    objects.push_back(new Cube(glm::vec3(-1.3f, -1.3f, -2.5f), glm::vec3(1.3f, 1.3f, -0.5f), rubber));
    //--necklace
    objects.push_back(new Cube(glm::vec3(-1.3f, -1.3f, -0.5f), glm::vec3(1.3f, 1.3f, -0.499f), red));
    //--pixels
    objects.push_back(new Cube(glm::vec3(-1.301f, 1.0f, -2.5f), glm::vec3(1.301f, 1.3f, -2.2f), graySecond));
    objects.push_back(new Cube(glm::vec3(-1.301f, 0.7f, -2.5f), glm::vec3(1.301f, 1.0f, -2.2f), grayThird));
    objects.push_back(new Cube(glm::vec3(-1.301f, 0.7f, -2.2f), glm::vec3(1.301f, 1.0f, -1.1f), graySecond));
    objects.push_back(new Cube(glm::vec3(-1.301f, 0.4f, -2.5f), glm::vec3(1.301f, 0.7f, -2.2f), grayFourth));
    objects.push_back(new Cube(glm::vec3(-1.301f, 0.4f, -2.2f), glm::vec3(1.301f, 0.7f, -1.9f), grayThird));
    objects.push_back(new Cube(glm::vec3(-1.301f, 0.4f, -2.5f), glm::vec3(1.301f, 0.1f, -2.2f), grayThird));

    objects.push_back(new Cube(glm::vec3(-1.302f, 1.3f, -0.8f), glm::vec3(1.302f, 1.0f, -0.5f), grayThird));
    objects.push_back(new Cube(glm::vec3(-1.302f, 1.0f, -1.1f), glm::vec3(1.302f, 0.7f, -0.8f), grayFourth));
    objects.push_back(new Cube(glm::vec3(-1.301f, 1.3f, -0.8f), glm::vec3(1.301f, -0.4f, -0.5f), graySecond));

    objects.push_back(new Cube(glm::vec3(-1.301f, -1.0f, -2.2f), glm::vec3(1.301f, -0.7f, -0.9f), graySecond));
    objects.push_back(new Cube(glm::vec3(-1.302f, -1.0f, -1.2f), glm::vec3(1.302f, -0.7f, -0.9f), grayFourth));
    objects.push_back(new Cube(glm::vec3(-1.301f, -1.0f, -0.9f), glm::vec3(1.301f, -0.7f, -0.6f), grayThird));
    objects.push_back(new Cube(glm::vec3(-1.301f, -0.7f, -1.2f), glm::vec3(1.301f, -0.4f, -0.9f), grayThird));
    objects.push_back(new Cube(glm::vec3(-1.301f, -1.3f, -1.2f), glm::vec3(1.301f, -1.0f, -0.9f), grayThird));

    objects.push_back(new Cube(glm::vec3(-1.301f, -1.3f, -2.5f), glm::vec3(1.301f, -0.7f, -2.2f), brownWhite));
    objects.push_back(new Cube(glm::vec3(-1.301f, -1.3f, -2.2f), glm::vec3(1.301f, -1.0f, -1.9f), brownWhite));

    //pixelsOnBody

    //Body-back
    objects.push_back(new Cube(glm::vec3(-1.0f, -1.3f, -5.5f), glm::vec3(1.0f, 0.85f, -2.5f), rubber));
    objects.push_back(new Cube(glm::vec3(-1.001f, 0.5f, -5.5f), glm::vec3(1.001f, 0.85f, -5.3f), grayFourth));
    objects.push_back(new Cube(glm::vec3(-1.001f, 0.5f, -5.3f), glm::vec3(1.001f, 0.85f, -4.0f), graySecond));
    objects.push_back(new Cube(glm::vec3(-1.001f, 0.5f, -5.5f), glm::vec3(1.001f, -0.1f, -5.3f), graySecond));

    objects.push_back(new Cube(glm::vec3(-1.001f, -1.3f, -2.5f), glm::vec3(1.001f, -1.0f, -5.0f), grayThird));
    objects.push_back(new Cube(glm::vec3(-1.001f, -1.0f, -2.5f), glm::vec3(1.001f, -0.7f, -4.4f), grayThird));
    objects.push_back(new Cube(glm::vec3(-1.001f, -1.0f, -2.5f), glm::vec3(1.001f, 0.5f, -2.8f), grayThird));
    objects.push_back(new Cube(glm::vec3(-1.002f, -1.3f, -2.5f), glm::vec3(1.002f, -1.0f, -2.8f), grayFourth));

    objects.push_back(new Cube(glm::vec3(-1.001f, 0.0f, -3.5f), glm::vec3(1.001f, 0.3f, -5.0f), graySecond));
    objects.push_back(new Cube(glm::vec3(-1.001f, -0.3f, -2.8f), glm::vec3(1.001f, -0.6f, -4.0f), graySecond));


    //Arms
    //----Front-right
    objects.push_back(new Cube(glm::vec3(0.15f, -1.3f, -0.95f), glm::vec3(0.85f, -3.5f, -1.65f), rubber));
    objects.push_back(new Cube(glm::vec3(0.15f, -2.6f, -0.949f), glm::vec3(0.85f, -3.2f, -1.651f), brownThird));

    //----Front-left
    objects.push_back(new Cube(glm::vec3(-0.15f, -1.3f, -0.95f), glm::vec3(-0.85f, -3.5f, -1.65f), rubber));
    objects.push_back(new Cube(glm::vec3(-0.15f, -2.6f, -0.949f), glm::vec3(-0.85f, -3.2f, -0.95f), brownThird));
    objects.push_back(new Cube(glm::vec3(-0.149f, -2.9f, -1.30f), glm::vec3(-0.851f, -3.2f, -1.65f), brownThird));
    objects.push_back(new Cube(glm::vec3(-0.149f, -2.9f, -0.95f), glm::vec3(-0.851f, -3.2f, -1.30f), brownWhite));
    objects.push_back(new Cube(glm::vec3(-0.149f, -2.9f, -1.30f), glm::vec3(-0.851f, -3.2f, -1.65f), brownWhite));
    objects.push_back(new Cube(glm::vec3(-0.149f, -2.6f, -1.30f), glm::vec3(-0.851f, -2.9f, -1.65f), brownWhite));
    objects.push_back(new Cube(glm::vec3(-0.149f, -2.6f, -0.95f), glm::vec3(-0.851f, -2.9f, -1.30f), brownThird));

    //objects.push_back(new Cube(glm::vec3(-0.151f, -2.6f, -0.95f), glm::vec3(-0.851f, -2.9f, -1.25f), brownWhite));

    //----Back-right
    objects.push_back(new Cube(glm::vec3(0.15f, -1.3f, -5.05f), glm::vec3(0.85f, -3.5f, -4.35f), rubber));
    objects.push_back(new Cube(glm::vec3(0.15f, -2.6f, -5.051f), glm::vec3(0.85f, -2.9f, -4.349f), brownThird));
    objects.push_back(new Cube(glm::vec3(0.15f, -2.9f, -5.051f), glm::vec3(0.50f, -3.2f, -4.349f), brownThird));

    objects.push_back(new Cube(glm::vec3(0.149f, -2.9f, -5.05f), glm::vec3(0.851f, -3.2f, -4.70f), brownThird));
    objects.push_back(new Cube(glm::vec3(0.149f, -2.9f, -4.70f), glm::vec3(0.851f, -3.2f, -4.35f), brownWhite));

    objects.push_back(new Cube(glm::vec3(0.149f, -2.6f, -5.05f), glm::vec3(0.851f, -2.9f, -4.70f), brownWhite));
    objects.push_back(new Cube(glm::vec3(0.149f, -2.6f, -4.70f), glm::vec3(0.851f, -2.9f, -4.35f), brownThird));

    //----Back-left
    objects.push_back(new Cube(glm::vec3(-0.15f, -1.3f, -5.05f), glm::vec3(-0.85f, -3.5f, -4.35f), rubber));
    objects.push_back(new Cube(glm::vec3(-0.15f, -2.6f, -5.051f), glm::vec3(-0.85f, -2.9f, -4.349f), brownThird));
    objects.push_back(new Cube(glm::vec3(-0.85f, -2.9f, -5.051f), glm::vec3(-0.50f, -3.2f, -4.349f), brownThird));

    objects.push_back(new Cube(glm::vec3(-0.149f, -2.9f, -5.05f), glm::vec3(-0.851f, -3.2f, -4.70f), brownThird));
    objects.push_back(new Cube(glm::vec3(-0.149f, -2.9f, -4.70f), glm::vec3(-0.851f, -3.2f, -4.35f), brownWhite));

    objects.push_back(new Cube(glm::vec3(-0.149f, -2.6f, -5.05f), glm::vec3(-0.851f, -2.9f, -4.70f), brownWhite));
    objects.push_back(new Cube(glm::vec3(-0.149f, -2.6f, -4.70f), glm::vec3(-0.851f, -2.9f, -4.35f), brownThird));

    //tail
    objects.push_back(new Cube(glm::vec3(-0.35f, 0.0f, -5.5f), glm::vec3(0.35f, 0.70f, -8.5f), rubber));


    //objects.push_back(new Cube(glm::vec3(1.0f, 1.0f, -3.0f), glm::vec3(1.5f, 1.5f, 1.5f), mirror));
    //objects.push_back(new Sphere(glm::vec3(-1.0f, 0.0f, -4.0f), 1.0f, ivory));
    //objects.push_back(new Cube(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), mirror));
    //objects.push_back(new Cube(glm::vec3(0.0f, 1.0f, -3.0f), 1.0f, glass));
}

void render() {
    float fov = 3.1415/3;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {

            float random_value = static_cast<float>(std::rand())/static_cast<float>(RAND_MAX);
            if (random_value < 0.0) {
                continue;
            }



            float screenX = (2.0f * (x + 0.5f)) / SCREEN_WIDTH - 1.0f;
            float screenY = -(2.0f * (y + 0.5f)) / SCREEN_HEIGHT + 1.0f;
            screenX *= ASPECT_RATIO;
            screenX *= tan(fov/2.0f);
            screenY *= tan(fov/2.0f);


            glm::vec3 cameraDir = glm::normalize(camera.target - camera.position);

            glm::vec3 cameraX = glm::normalize(glm::cross(cameraDir, camera.up));
            glm::vec3 cameraY = glm::normalize(glm::cross(cameraX, cameraDir));
            glm::vec3 rayDirection = glm::normalize(
                    cameraDir + cameraX * screenX + cameraY * screenY
            );

            Color pixelColor = castRay(camera.position, rayDirection);
            /* Color pixelColor = castRay(glm::vec3(0,0,20), glm::normalize(glm::vec3(screenX, screenY, -1.0f))); */

            point(glm::vec2(x, y), pixelColor);
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Raytracer - FPS: 0",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    bool running = true;
    SDL_Event event;

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = startTime;

    setUp();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        print("up");
                        camera.rotate(0.0f, 1.0f);
                        break;
                    case SDLK_DOWN:
                        print("down");
                        camera.rotate(0.0f, -1.0f);
                        break;
                    case SDLK_LEFT:
                        print("left");
                        camera.rotate(-1.0f, 0.0f);
                        break;
                    case SDLK_RIGHT:
                        print("right");
                        camera.rotate(1.0f, 0.0f);
                        break;
                    case SDLK_w:
                        camera.move(1.0f);
                        break;
                    case SDLK_s:
                        camera.move(-1.0f);
                        break;


                }
            }


        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render();

        // Present the renderer
        SDL_RenderPresent(renderer);

        frameCount++;

        // Calculate and display FPS
        if (SDL_GetTicks() - currentTime >= 1000) {
            currentTime = SDL_GetTicks();
            std::string title = "Raytracer - FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}