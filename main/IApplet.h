#pragma once
#include "system.h"
#include "JaDraw.h"
class MatrixPanel_I2S_DMA;

struct InputData {
    int rotation = 0; 
    bool pressed = false;
};

class IApplet {
public:
    virtual ~IApplet() = default;
    /// @brief Called once the applet is selected, before running any loop.
    virtual void setup() = 0;
    /// @brief Called every frame.
    /// @param canvas The framebuffer to show up on the screen.
    /// @param dt Time since last call, in seconds.
    /// @param inputs The controls that may be consumed (e.g. knob turns, button presses).
    
    virtual void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) = 0;
    virtual const char* getName() const = 0;
};
