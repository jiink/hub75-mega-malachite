#include <Arduino.h>
#include <stdint.h>
#include "system.h"
#include "movingPixel.h"
#include "phases.h"
#include "city.h"

/*  Default library pin configuration for the reference
  you can redefine only ones you need later on object creation
#define R1 25
#define G1 26
#define BL1 27
#define R2 14
#define G2 12
#define BL2 13
#define CH_A 23
#define CH_B 19
#define CH_C 5
#define CH_D 17
#define CH_E -1 // assign to any available pin if using panels with 1/32 scan
#define CLK 16
#define LAT 4
#define OE 15
*/

#define MAX_APPLETS 16

// Prototypes
void doNothing(void);

// Variables
MatrixPanel_I2S_DMA* matrix = nullptr;

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
	strncpy(applet2.name, "Phases", sizeof(applet2.name) - 1);
	applet2.appletSetup = &phasesSetup;
	applet2.appletLoop = &phasesLoop;
	applets[numApplets++] = applet2;

	// Set up the third applet
	static AppletEntry applet3;
	strncpy(applet3.name, "City", sizeof(applet3.name) - 1);
	applet3.appletSetup = &citySetup;
	applet3.appletLoop = &cityLoop;
	applets[numApplets++] = applet3;

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
    //matrix->flipDMABuffer(); // Show the back buffer, set currently output buffer to the back (i.e. no longer being sent to LED panels)
    //matrix->clearScreen();   // Now clear the back-buffer
	matrix->fillScreen(matrix->color565(128, 0, 50));

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
	matrix->setTextColor(matrix->color565(255, 255, 255));
	matrix->setCursor(4, 4);
	matrix->print(applets[appletSelectedIndex].name);
	
    matrix->flipDMABuffer(); 
}

void setup()
{
    // TODO: Choose pins and solder on connector for rotary knob
	// pinMode(ROTARY_CLK, INPUT_PULLUP);
	// pinMode(ROTARY_DT, INPUT_PULLUP);
	// pinMode(ROTARY_SW, INPUT_PULLUP);
	// attachInterrupt(digitalPinToInterrupt(ROTARY_CLK), rotaryEncoderInterrupt, RISING);
	// attachInterrupt(digitalPinToInterrupt(ROTARY_SW), buttonInterrupt, FALLING);

	Serial.begin(9600);
    HUB75_I2S_CFG mxconfig(PANEL_WIDTH, PANEL_HEIGHT, PANELS_NUMBER);
    mxconfig.double_buff = true;
    matrix = new MatrixPanel_I2S_DMA(mxconfig);
    matrix->begin();
    matrix->setBrightness8(255);
	//matrix->setTextSize(1);

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
