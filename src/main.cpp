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
uint16_t backgroundColor = matrix->color565(128, 0, 50);

enum ProgramState
{
	MENU,
	APPLET
};
ProgramState state = APPLET;

struct AppletEntry
{
	char name[64] = "None";
	void (*appletSetup)(void);
	void (*appletLoop)(void);
};
// Array to store the available applets
AppletEntry applets[MAX_APPLETS];
uint8_t numApplets = 0;
uint8_t appletSelectedIndex = 3;

void doNothing()
{
	return;
}

void addApplet(const char* name, void (*appletSetup)(void), void (*appletLoop)(void))
{
    // Set up the first applet
    static AppletEntry applet;
    strncpy(applet.name, name, sizeof(applet.name) - 1);
    applet.appletSetup = appletSetup;
    applet.appletLoop = appletLoop;
    applets[numApplets++] = applet;
}

// Declare and define a global constructor function
void initializeApplets()
{
    addApplet("Moving Pixel", &movingPixelSetup, &movingPixelLoop);
    addApplet("Phases", &phasesSetup, &phasesLoop);
    addApplet("City", &citySetup, &cityLoop);
    addApplet("Life", &lifeSetup, &lifeLoop);
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

	if (rotationInput0 > 0)
	{
		rotationInput0 = 0;
		appletSelectedIndex = addWrap(appletSelectedIndex, numApplets - 1, 1);
        //Serial.println("P");
        Serial.printf("Selected applet: %d\r\n", appletSelectedIndex);
	}
	else if (rotationInput0 < 0)
	{
		rotationInput0 = 0;
		appletSelectedIndex = addWrap(appletSelectedIndex, numApplets - 1, -1);
        //Serial.println("N");
        Serial.printf("Selected applet: %d\r\n", appletSelectedIndex);
	}

	if (buttonPressed0)
	{
        Serial.printf("Button pressed!\r\n");
		buttonPressed0 = false;
		state = APPLET;
		applets[appletSelectedIndex].appletSetup();
	}
    if (buttonPressed1)
    {
        if (backgroundColor == matrix->color565(128, 0, 50))
        {
            backgroundColor = matrix->color565(0, 0, 0);
        }
        else
        {
            backgroundColor = matrix->color565(128, 0, 50);
        }
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
    rotaryEncoderSetup();

	initializeApplets();

    connectToWifi();
    synchrnoizeTime();
    Serial.println("Connected to WiFi and synchronized time.");

    if (state == APPLET)
    {
        applets[appletSelectedIndex].appletSetup();
    }
}

void loop()
{
    handleOtaPeriodic();
	switch (state)
	{
	case MENU:
		menuLoop();
		break;
	case APPLET:
		if (buttonPressed0)
		{
			buttonPressed0 = false;
			state = MENU;
		}
		applets[appletSelectedIndex].appletLoop();
		break;
	}
}

