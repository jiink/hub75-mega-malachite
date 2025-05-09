#pragma once
#include "IApplet.h"
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

class SimpleClock : public IApplet {
public:
    SimpleClock();
    ~SimpleClock() override = default;
    void setup() override;
    void loop(MatrixPanel_I2S_DMA* matrix, float dt, const InputData& inputs) override;
    const char* getName() const override;
};

SimpleClock::SimpleClock() { }

void SimpleClock::setup() { }

void SimpleClock::loop(MatrixPanel_I2S_DMA* matrix, float dt, const InputData& inputs) {
    matrix->fillScreenRGB888(20, 5, 0);
    matrix->drawPixelRGB888(40, 20, 0, 255, 0);
    matrix->drawPixelRGB888(41, 21, 0, 255, 0);
    matrix->drawPixelRGB888(40, 21, 0, 255, 0);
    matrix->flipDMABuffer();
}

const char* SimpleClock::getName() const {
    return "Simple Clock";
}
