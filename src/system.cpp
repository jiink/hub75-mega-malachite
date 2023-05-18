#include "system.h"

volatile int rotationInput = 0;
volatile bool buttonPressed = false;

// Interrupts
void rotaryEncoderInterrupt()
{
	if (digitalRead(ROTARY_DT))
	{
		rotationInput--;
	}
	else
	{
		rotationInput++;
	}
}

void buttonInterrupt()
{
	buttonPressed = true;
}