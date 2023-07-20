#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include "xtensa/core-macros.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32
#define PANELS_NUMBER 1
#define SCREEN_W PANEL_WIDTH

#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE 9
#define LAT 10
#define A A0
#define B A1
#define C A2
#define D A3

// Rotary knob pins
#define ROTARY_CLK 20
#define ROTARY_DT 53
#define ROTARY_SW 21

extern MatrixPanel_I2S_DMA* matrix;

extern volatile bool buttonPressed;
extern volatile int rotationInput; // Negative is CCW, positive is CW. Resets to 0 when input consumed.

// Interrupts
void rotaryEncoderInterrupt(void);
void buttonInterrupt(void);

#endif
