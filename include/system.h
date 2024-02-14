#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include "xtensa/core-macros.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>
#include <Esp32RotaryEncoder.h>

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32
#define PANELS_NUMBER 1
#define SCREEN_W PANEL_WIDTH

// Rotary knob pins
// #define ROTARY_CLK 23
// #define ROTARY_DT 22
// #define ROTARY_SW 1
#define ROTARY_ENCODER_0_A_PIN 36      // CLK
#define ROTARY_ENCODER_0_B_PIN 39      // DT
#define ROTARY_ENCODER_0_BUTTON_PIN 34 // SW
#define ROTARY_ENCODER_0_VCC_PIN -1
#define ROTARY_ENCODER_0_STEPS 4
#define ROTARY_ENCODER_1_A_PIN 35      // CLK
#define ROTARY_ENCODER_1_B_PIN 32      // DT
#define ROTARY_ENCODER_1_BUTTON_PIN 33 // SW
#define ROTARY_ENCODER_1_VCC_PIN -1
#define ROTARY_ENCODER_1_STEPS 4

extern MatrixPanel_I2S_DMA* matrix;
extern RotaryEncoder rotaryEncoder0;
extern RotaryEncoder rotaryEncoder1;

extern volatile bool buttonPressed0;
extern volatile bool buttonPressed1;
extern volatile int rotationInput0; // Negative is CCW, positive is CW. Resets to 0 when input consumed.
extern volatile int rotationInput1;

// Interrupts
// void rotaryEncoderInterrupt(void);
// void buttonInterrupt(void);
void rotaryEncoderSetup();
void connectToWifi();
void handleOtaPeriodic();
void synchrnoizeTime();
#endif
