// This applet lets you change settings
// like the LED board's brightness, etc.
// You use the knob to pick between settings, click to select it,
// and then use the knob to change the setting, and click to confirm.
#include <stdint.h>
#include <math.h>
#include "system.h"
#include "settingsChannel.h"

// right now there's just 1 setting (brightness) so screw it
uint8_t currentBrightness = 255;

void settingsChannelSetup(void)
{

}

void settingsChannelLoop(void)
{
    // Handle input
    if (rotationInput0 > 0)
    {
        rotationInput0 = 0;
        currentBrightness = min(currentBrightness + 16, 255);
    }
    else if (rotationInput0 < 0)
    {
        rotationInput0 = 0;
        currentBrightness = max(currentBrightness - 16, 4);
    }
    matrix->setBrightness(currentBrightness);
    // Draw
    matrix->fillScreen(matrix->color333(0, 2, 5));
    matrix->setCursor(0, 0);
    matrix->setTextColor(matrix->color333(7, 7, 7));
    char buffer[32];
    matrix->print("Settings");
    matrix->setCursor(0, 8);
    sprintf(buffer, "Brightness: %d", currentBrightness);
    matrix->print(buffer);

    matrix->flipDMABuffer();
}