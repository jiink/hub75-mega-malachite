#include <math.h>
#include "system.h"

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

volatile int rotationInput = 0;
volatile bool buttonPressed = false;

static int lastEncoderValue = 0;
static unsigned long lastTimePressed = 0;

void rotaryLoop()
{
    //dont print anything unless value changed
    if (rotaryEncoder.encoderChanged())
    {
        int currentValue = rotaryEncoder.readEncoder();
        rotationInput += currentValue - lastEncoderValue;
        lastEncoderValue = currentValue;
        Serial.printf("rotationInput: %d\r\n", rotationInput);
    }
    if (rotaryEncoder.isEncoderButtonClicked())
    {
        printf("m - l: %d - %d\r\n", millis(), lastTimePressed);
        // Soft debouncing
        if (abs((int)(millis() - lastTimePressed)) > 200)
        {
            printf("Pressed\r\n");
            buttonPressed = true;
            lastTimePressed = millis();
        }
    }
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}