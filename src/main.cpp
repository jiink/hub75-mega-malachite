#include <Arduino.h>
#include <stdint.h>
#include "system.h"
#include "applets\movingPixel.h"
#include "applets\phases.h"
#include "applets\city.h"
#include "applets\life.h"

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

	static AppletEntry applet4;
	strncpy(applet3.name, "Life", sizeof(applet4.name) - 1);
	applet3.appletSetup = &lifeSetup;
	applet3.appletLoop = &lifeLoop;
	applets[numApplets++] = applet4;

	// ... Add more applets if needed
}


int addWrap(int num, int max, int incr) {
    // Calculate the result after adding incr to num
    int result = num + incr;

    // Calculate the range size (max + 1) to ensure wrapping occurs in both directions
    int rangeSize = max + 1;

    // Calculate the modular arithmetic result to handle wrapping in both directions
    // If the result is negative, add the range size to wrap around from the end to the start.
    // If the result is greater than or equal to the range size, subtract the range size to wrap around from the start to the end.
    result = (result % rangeSize + rangeSize) % rangeSize;

    return result;
}

void menuLoop()
{
	matrix->fillScreen(matrix->color565(128, 0, 50));

	if (rotationInput > 0)
	{
		rotationInput = 0;
		appletSelectedIndex = addWrap(appletSelectedIndex, numApplets - 1, 1);
        //Serial.println("P");
        Serial.printf("Selected applet: %d\r\n", appletSelectedIndex);
	}
	else if (rotationInput < 0)
	{
		rotationInput = 0;
		appletSelectedIndex = addWrap(appletSelectedIndex, numApplets - 1, -1);
        //Serial.println("N");
        Serial.printf("Selected applet: %d\r\n", appletSelectedIndex);
	}

	if (buttonPressed)
	{
        Serial.printf("Button pressed!\r\n");
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
	Serial.begin(115200);
    Serial.println("Starting up...");
    HUB75_I2S_CFG mxconfig(PANEL_WIDTH, PANEL_HEIGHT, PANELS_NUMBER);
    mxconfig.double_buff = true;
    matrix = new MatrixPanel_I2S_DMA(mxconfig);
    matrix->begin();
    matrix->setBrightness8(255);

    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    //set boundaries and if values should cycle or not
    bool circleValues = false;
    rotaryEncoder.setBoundaries(-1000000, 1000000, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
    //rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
    rotaryEncoder.disableAcceleration();

	initializeApplets();
}

void loop()
{
    rotaryLoop();
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

