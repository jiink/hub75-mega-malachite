#include <Arduino.h>
#include <RGBmatrixPanel.h>
#include <stdint.h>
#include "system.h"
#include "movingPixel.h"

#define MAX_APPLETS 16

// Prototypes
void doNothing(void);

// Variables
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true, 64);

enum ProgramState
{
	MENU,
	APPLET
};
ProgramState state = MENU;

struct AppletEntry
{
	char name[64] = "None";
	void (*appletSetup)(void);
	void (*appletLoop)(void);
};
// Array to store the available applets
AppletEntry applets[MAX_APPLETS];
uint8_t numApplets = 0;
uint8_t appletSelectedIndex = 0;

void doNothing()
{
	return;
}



// Declare and define a global constructor function
void initializeApplets()
{
	// Set up the first applet
	static AppletEntry applet1;
	strncpy(applet1.name, "Moving Pixel", sizeof(applet1.name) - 1);
	applet1.appletSetup = &movingPixelSetup;
	applet1.appletLoop = &movingPixelLoop;
	applets[numApplets++] = applet1;

	// Set up the second applet
	static AppletEntry applet2;
	strncpy(applet2.name, "Applet 2", sizeof(applet2.name) - 1);
	applet2.appletSetup = &doNothing;
	applet2.appletLoop = &doNothing;
	applets[numApplets++] = applet2;

	// ... Add more applets if needed
}

uint8_t incrementWrap(uint8_t num, uint8_t max, uint8_t incr)
{
	int work = num;
	work += incr;
	if (work < 0)
	{
		work = max;
	}
	if (work > max)
	{
		work = 0;
	}
	return (uint8_t)work;
}

void menuLoop()
{
	matrix.fillScreen(matrix.ColorHSV(millis() / 16, 255, 100, true));

	if (rotationInput > 0)
	{
		rotationInput = 0;
		appletSelectedIndex = incrementWrap(appletSelectedIndex, numApplets - 1, 1);
	}
	else if (rotationInput < 0)
	{
		rotationInput = 0;
		appletSelectedIndex = incrementWrap(appletSelectedIndex, numApplets - 1, -1);
	}

	if (buttonPressed)
	{
		buttonPressed = false;
		state = APPLET;
		applets[appletSelectedIndex].appletSetup();
	}

	// Display
	// Show the name of the selected applet
	matrix.setTextColor(matrix.Color333(7, 7, 7));
	matrix.setCursor(4, 4);
	matrix.print(applets[appletSelectedIndex].name);
	
	matrix.swapBuffers(true);
}

void setup()
{
	pinMode(ROTARY_CLK, INPUT);
	pinMode(ROTARY_DT, INPUT);
	pinMode(ROTARY_SW, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(ROTARY_CLK), rotaryEncoderInterrupt, RISING);
	attachInterrupt(digitalPinToInterrupt(ROTARY_SW), buttonInterrupt, FALLING);

	Serial.begin(9600);
	matrix.begin();

	matrix.setTextSize(1);

	initializeApplets();
}

void loop()
{
	switch (state)
	{
	case MENU:
		menuLoop();
		break;
	case APPLET:
		if (buttonPressed)
		{
			buttonPressed = false;
			state = MENU;
		}
		applets[appletSelectedIndex].appletLoop();
		break;
	}
}
