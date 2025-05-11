#pragma once
#include "IApplet.h"
#include "JaDraw.h"
#include "vmath_all.hpp"
using vmath_hpp::fvec2;
using vmath_hpp::fvec3;

class Simplex : public IApplet {
public:
    Simplex();
    ~Simplex() override = default;
    void setup() override;
    void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) override;
    const char* getName() const override;
private:
    float x = 0.0f;
    float y = 0.0f;
    float t = 0.0f;
};

Simplex::Simplex() { }

void Simplex::setup() { }

static uint32_t fvec3ToRGBAa(fvec3& floatColor)
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

void Simplex::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    t += dt;
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        int fragCoordX = i % WIDTH;
        int fragCoordY = i / WIDTH;
        fvec2 fragCoord(fragCoordX, fragCoordY);
        fvec3 color(0, 0, 0);
        float j = 0;
        float z = 0;
        float d = 0;
        for (color *= j; j++<15.0f; )
        {
            fvec3 p = z * vmath_hpp::normalize(fvec3(fragCoord + fragCoord, 0) - fvec3(WIDTH, HEIGHT, HEIGHT));
            fvec3 v(0, 0, 0);
            p.z -= t;
            fvec3 sinp = sin(p);
            v = cos(p) - fvec3(sinp.y, sinp.z, sinp.x);
            d = 0.0001f + 0.5f * vmath_hpp::length(std::max(v, fvec3(v.y, v.z, v.x) * 0.2f));
            z += d;
            color += (cos(p) + 1.2f) / d;
        }
        color = color / (color + 1000.0f);
        canvas.drawPixel(fragCoordX, fragCoordY, fvec3ToRGBAa(color));
    }
}

const char* Simplex::getName() const {
    return "Simplex";
}
