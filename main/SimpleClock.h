#pragma once
#include "IApplet.h"
#include "JaDraw.h"

class SimpleClock : public IApplet {
public:
    SimpleClock();
    ~SimpleClock() override = default;
    void setup() override;
    void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) override;
    const char* getName() const override;
private:
    float x = 0.0f;
    float y = 0.0f;
    float t = 0.0f;
};

SimpleClock::SimpleClock() { }

void SimpleClock::setup() { }

void SimpleClock::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    t += dt;
    const float speed = 10.0f;
    x += speed * dt * 2.3f;
    y += speed * dt * 1.5f;
    if (x >= WIDTH) x = 0;
    if (y >= HEIGHT) y = 0;
    canvas.clear(0x001030FF);
    canvas.drawLineAA(0, 0, x, y, Colors::Red);
    canvas.drawLineAA(WIDTH - 1, 0, x, y, Colors::Orange);
    canvas.drawText("hello?",
        8 + 16 * sin(t * 1.0f),
        10 + 10 * cos(t * 0.8f),
        2.0f + 1.0f * cos(t * 0.5),
        Colors::Magenta);
}

const char* SimpleClock::getName() const {
    return "Simple Clock";
}
