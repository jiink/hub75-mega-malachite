#pragma once
class MatrixPanel_I2S_DMA;

struct InputData {
    int rotation = 0; 
    bool pressed = false;
};

class IApplet {
public:
    virtual ~IApplet() = default;
    /// @brief Called once the applet is selected, before running any loop.
    /// @param matrix The screen.
    virtual void setup() = 0;
    /// @brief Called every frame.
    /// @param matrix The screen.
    /// @param dt Time since last call, in seconds.
    /// @param inputs The controls that may be consumed (e.g. knob turns, button presses).
    virtual void loop(MatrixPanel_I2S_DMA* matrix, float dt, const InputData& inputs) = 0;
    virtual const char* getName() const = 0;
};
