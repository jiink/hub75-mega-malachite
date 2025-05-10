#pragma once
#include "IApplet.h"
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
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
};

SimpleClock::SimpleClock() { }

void SimpleClock::setup() { }

void SimpleClock::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    const float speed = 10.0f;
    x += speed * dt;
    y += speed * dt;
    if (x >= WIDTH) x = 0;
    if (y >= HEIGHT) y = 0;
    canvas.clear(0x001030FF);
    canvas.drawLineAA(0, 0, x, y, Colors::Green);
}

const char* SimpleClock::getName() const {
    return "Simple Clock";
}
