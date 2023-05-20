#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include <RGBmatrixPanel.h>

#define SCREEN_W 64

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

extern RGBmatrixPanel matrix;

extern volatile bool buttonPressed;
extern volatile int rotationInput; // Negative is CCW, positive is CW. Resets to 0 when input consumed.

// Interrupts
void rotaryEncoderInterrupt(void);
void buttonInterrupt(void);

#endif
