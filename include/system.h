#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include "xtensa/core-macros.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>
#include "AiEsp32RotaryEncoder.h"

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32
#define PANELS_NUMBER 1
#define SCREEN_W PANEL_WIDTH

// Rotary knob pins
// #define ROTARY_CLK 23
// #define ROTARY_DT 22
// #define ROTARY_SW 1
#define ROTARY_ENCODER_A_PIN 36
#define ROTARY_ENCODER_B_PIN 39
#define ROTARY_ENCODER_BUTTON_PIN 34
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4

extern MatrixPanel_I2S_DMA* matrix;
extern AiEsp32RotaryEncoder rotaryEncoder;

extern volatile bool buttonPressed;
extern volatile int rotationInput; // Negative is CCW, positive is CW. Resets to 0 when input consumed.

// Interrupts
// void rotaryEncoderInterrupt(void);
// void buttonInterrupt(void);
void rotaryLoop();
void IRAM_ATTR readEncoderISR();
#endif
