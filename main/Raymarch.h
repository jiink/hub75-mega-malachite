#pragma once
#include "IApplet.h"
#include "JaDraw.h"
#include "vmath_all.hpp"

class Raymarch : public IApplet {
public:
    Raymarch();
    ~Raymarch() override = default;
    void setup() override;
    void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) override;
    const char* getName() const override;
private:
    float x = 0.0f;
    float y = 0.0f;
    float t = 0.0f;
};

Raymarch::Raymarch() { }

void Raymarch::setup() { }

using vmath_hpp::fvec2;
using vmath_hpp::fvec3;
/**
 * @brief Converts a floating-point color to a 32-bit RGBA integer.
 * @param floatColor Reference to the floating-point color structure.
 * @return 32-bit unsigned integer representing the RGBA color.
 */
static uint32_t fvec3ToRGBA_(fvec3& floatColor)
{
    if (floatColor.x < 0.0f) { floatColor.x = 0.0f; }
    if (floatColor.x > 1.0f) { floatColor.x = 1.0f; }
    if (floatColor.y < 0.0f) { floatColor.y = 0.0f; }
    if (floatColor.y > 1.0f) { floatColor.y = 1.0f; }
    if (floatColor.z < 0.0f) { floatColor.z = 0.0f; }
    if (floatColor.z > 1.0f) { floatColor.z = 1.0f; }
    uint8_t r = static_cast<uint8_t>(floatColor.x * 255.0f);
    uint8_t g = static_cast<uint8_t>(floatColor.y * 255.0f);
    uint8_t b = static_cast<uint8_t>(floatColor.z * 255.0f);
    uint8_t a = 255;
    return JADRAW_RGBA(r, g, b, a);
}

static fvec3 spherePos(0.0f, 1.0f, 6.0f);
static float sphereRadius = 1.0f;
static fvec3 lightPos(0, 0, 6.5);
//static fvec3 lightColor()

// Samples the 3D SDF and returns the distance between
// the given position and the defined geometery.
static float getDist(fvec3 pos)
{
    float planeDist = pos.y;
    float sphereDist = vmath_hpp::length(pos - spherePos) - sphereRadius;
    return std::min(planeDist, sphereDist);
}

static fvec3 getNormal(fvec3 pos)
{
    return vmath_hpp::normalize(spherePos - pos);
}

// Raymarches the scene and returns the final distance from cam to that point
static float raymarch(fvec3 cam, fvec3 dir)
{
    float dist = 0;
    const int maxIter = 100;
    for (int i = 0; i < maxIter; i++)
    {
        fvec3 pos = cam + dir * dist;
        float stepDist = getDist(pos);
        dist += stepDist;
        const float maxDist = 100.0f;
        const float minDist = 0.01f;
        if (dist > maxDist || dist < minDist) { break; }
    }
    return dist;
}

static bool alternate = true;
void Raymarch::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    t += dt;
    spherePos.y = 1.2 + 0.5f * sin(t);
    sphereRadius = 1.0 + 0.5 * cos(t*1.5);
    const float inverseHeight = 1.0f / static_cast<float>(HEIGHT);
    alternate = !alternate;
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        int fragCoordX = i % WIDTH;
        int fragCoordY = i / WIDTH;
        bool doIt = i % 2 == 0;
        if (fragCoordY % 2 == 0) { doIt = !doIt; }
        if (alternate) { doIt = !doIt; }
        if (!doIt) { continue; }
        fvec2 uv(
            (fragCoordX - WIDTH * 0.5f) * inverseHeight,
            -(fragCoordY - HEIGHT * 0.5f) * inverseHeight
        );
        fvec3 cam(0, 1, 1);
        fvec3 ray(uv.x, uv.y, cam.z);
        float dist = raymarch(cam, vmath_hpp::normalize(ray));
        fvec3 color(0, 0, 0);
        if (dist < 90.0f)
        {
            fvec3 hitPt = cam + ray * dist;
            fvec3 norm = getNormal(hitPt);
            fvec3 lightDir = vmath_hpp::normalize(lightPos - hitPt);
            float diffuseIntensity = std::max(0.0f, vmath_hpp::dot(norm, lightDir));
            static const fvec3 lightCol(1.0, 1.0, 0.8);
            color = color + diffuseIntensity * lightCol;
        }
        canvas.drawPixel(fragCoordX, fragCoordY, fvec3ToRGBA_(color));
    }
}

const char* Raymarch::getName() const {
    return "Raymarch";
}
