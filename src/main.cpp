#include <Arduino.h>
#include <stdint.h>
#include "system.h"
#include "applets\movingPixel.h"
#include "applets\phases.h"
#include "applets\city.h"
#include "applets\life.h"
#include "applets\simpleClock.h"
#include "applets\weatherChannel.h"
#include "applets\timer.h"

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
	APPLET,
    NOISE
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
uint8_t appletSelectedIndex = 0;

// TV static effect stuff
#define TVSTATIC_INTERVAL 100
#define STATIC_MEM_RANGE 16384
unsigned long tvStaticTimer = 0;
char* programMemoryStart = (char*)esp_get_idf_version();
char* noiseSource = (char*)programMemoryStart;

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
    addApplet("Simple Clock", &simpleClockSetup, &simpleClockLoop);
    addApplet("Moving Pixel", &movingPixelSetup, &movingPixelLoop);
    addApplet("Phases", &phasesSetup, &phasesLoop);
    addApplet("City", &citySetup, &cityLoop);
    addApplet("Life", &lifeSetup, &lifeLoop);
    addApplet("Weather Channel", &weatherChannelSetup, &weatherChannelLoop);
    addApplet("Timer", &timerSetup, &timerLoop);
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


ProgramState handleSystemControls(ProgramState s)
{
    if (s != APPLET) { return s; }
    // Handle the rotary encoder input
    // (Don't skip any)
    int prevSelectedIndex = appletSelectedIndex;
    if (rotationInput1 > 0)
    {
        rotationInput1 = 0;
        appletSelectedIndex = addWrap(appletSelectedIndex, numApplets - 1, 1);
        tvStaticTimer = millis();
        s = NOISE;
    }
    else if (rotationInput1 < 0)
    {
        rotationInput1 = 0;
        appletSelectedIndex = addWrap(appletSelectedIndex, numApplets - 1, -1);
        tvStaticTimer = millis();
        s = NOISE;
    }
    if (prevSelectedIndex != appletSelectedIndex)
    {
        applets[appletSelectedIndex].appletSetup();
    }
    return s;
}

ProgramState tvStaticLoop()
{
    matrix->fillScreen(backgroundColor);
    
    
    for (int y = 0; y < matrix->height(); y++)
    {
        for (int x = 0; x < matrix->width(); x++)
        {
            // Use program memory byte as grayscale value
            uint8_t grayValue = *noiseSource;
            matrix->drawPixel(x, y, matrix->color333(grayValue, grayValue, grayValue));

            // Move to the next byte, wrap around if necessary
            noiseSource++;
            if (noiseSource - programMemoryStart >= STATIC_MEM_RANGE)
            {
                noiseSource = programMemoryStart;
            }
        }
    }
    delay(20);
    matrix->flipDMABuffer();
    if (millis() - tvStaticTimer > TVSTATIC_INTERVAL)
    {
        tvStaticTimer = millis();
        return APPLET;
    }
    return NOISE;
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
    state = handleSystemControls(state);
	switch (state)
	{
        case APPLET:
            applets[appletSelectedIndex].appletLoop();
            break;
        case NOISE:
            state = tvStaticLoop();
            break;
	}
}

