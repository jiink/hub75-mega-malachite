#include "system.h"
#include "weatherChannel.h"
#include "digitsBig.h"
#include "sprites.h"

enum State
{
    STATE_FETCHING,
    STATE_DISPLAYING,
};

enum Condition
{
    CONDITION_CLEAR,
    CONDITION_CLOUDY,   
};

// The weather data actually displayed to the user
struct WeatherDisplay
{
    int temp;
    Condition condition;
};
WeatherDisplay weatherDisplay;
State state = STATE_FETCHING;
unsigned long fetchTimer = 0;
const uint16_t bgCol = matrix->color565(189, 234, 255);

// x and y are top left corner
static void drawFrame(int x, int y, int w, int h)
{
    matrix->drawRect(x + 2, y + 2, w - 2, h - 2, matrix->color565(16, 104, 168)); // Shadow
    matrix->drawRect(x + 0, y + 0, w - 1, h - 1, matrix->color565(16, 104, 168)); // Light
    matrix->drawRect(x + 1, y + 1, w - 1, h - 1, matrix->color565(16, 104, 168)); // Dark
}

static void drawFrameCentered(int x, int y, int w, int h)
{
    drawFrame(x - w / 2, y - h / 2, w, h);
}

static void fetchUpdate()
{
    const unsigned long FETCH_LENGTH = 2000;
    if (millis() - fetchTimer > FETCH_LENGTH)
    {
        state = STATE_DISPLAYING;
    }
    float amplitude = 36.0;
    float frameW = amplitude * sin(millis() / 200.0);


    drawFrameCentered(19, 15, frameW, 30);
}

static void drawDigitBig(int digit, int x, int y, uint16_t color)
{
    const uint8_t* digitData;
    if (digit < 0 || digit > 9)
    {
        digitData = DIGITBIG_0;
    }
    if (digit == 10)
    {
        digitData = DIGITBIG_DEGREE;
    }
    else
    {
        digitData = DIGITSBIG[digit];
    }
    for (int i = 0; i < DIGITBIG_HEIGHT; i++)
    {
        for (int j = 0; j < DIGITBIG_WIDTH; j++)
        {
            uint8_t rowBits = digitData[i];
            if (rowBits & (1 << (DIGITBIG_WIDTH - 1 - j)))  // Read the bits in reverse order
            {
                matrix->drawPixel(x + j, y + i, color);
            }
        }
    }
}

static void drawNumber(int number, int x, int y, uint16_t color)
{
    if (number == 0) {
        drawDigitBig(0, x, y, color);
        return;
    }

    int numDigits = (int)log10(number) + 1; // Calculate the number of digits
    int startX = x - (numDigits * DIGITBIG_WIDTH + (numDigits - 1)) + 1; // Calculate the starting x position

    for (int i = numDigits - 1; i >= 0; --i) {
        int digit = (number / (int)pow(10, i)) % 10; // Extract the current digit
        drawDigitBig(digit, startX, y, color);
        startX += DIGITBIG_WIDTH + 1; // Move to the next position
    }
}

static void drawTemperature(int temp, int x, int y, uint16_t color)
{
    drawNumber(temp, x, y, color);
    
}

void weatherChannelSetup(void)
{
    // get rid of this when using actual data from internet
    state = STATE_FETCHING; 
    weatherDisplay.temp = random(50, 100);
    weatherDisplay.condition = random(0, 2) == 0 ? CONDITION_CLEAR : CONDITION_CLOUDY;
    fetchTimer = millis();
}

void weatherChannelLoop(void)
{
    matrix->fillScreen(bgCol);
    switch (state)
    {
        case STATE_FETCHING:
            fetchUpdate();
            break;
        case STATE_DISPLAYING:
            drawFrame(1, 2, 36, 30);
            drawSprite(matrix, 2, 3, sunSprite);
            drawNumber(weatherDisplay.temp, 39, 6, matrix->color565(8, 56, 136));
            break;
    }
    matrix->flipDMABuffer();
}