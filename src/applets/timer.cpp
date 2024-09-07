#include "timer.h"
#include "system.h"
#include "digits.h"
#include "letters.h"

#define MIN_TO_MS(x) (x * 60 * 1000)

int timeLeft = MIN_TO_MS(5);
int startTimestamp = 0;
uint16_t digitColor = matrix->color565(255, 255, 255);

void timerSetup(void)
{
    startTimestamp = millis();
}

void timerLoop(void)
{
    while (rotationInput0 != 0)
    {
        int timeStep = 10 * 1000;
        if (rotationInput0 > 0)
        {
            timeLeft += timeStep;
            rotationInput0--;
        }
        else
        {
            timeLeft -= timeStep;
            rotationInput0++;
        }
    }

    int timeElapsed = millis() - startTimestamp;
    timeLeft -= timeElapsed;
    startTimestamp = millis();

    if (timeLeft <= 0)
    {
        timeLeft = 0;
    }

    int hours = timeLeft / 3600000;
    int minutes = (timeLeft % 3600000) / 60000;
    int seconds = (timeLeft % 60000) / 1000;
    const int timerYPos = 8;
    matrix->fillScreen(matrix->color565(0, 0, 0));
    draw2DigitBigNumber(hours, 0, timerYPos, digitColor);
    drawDigitBig(11, 16, timerYPos, digitColor);
    draw2DigitBigNumber(minutes, 24, timerYPos, digitColor);
    drawDigitBig(11, 40, timerYPos, digitColor);
    draw2DigitBigNumber(seconds, 48, timerYPos, digitColor);
    bool doAlarm = (timeLeft == 0);
    if (doAlarm)
    {
        if (millis() % 500 < 250)
        {
            matrix->fillScreen(matrix->color565(255, 255, 255));
            drawString("TIMES UP", 8, 4, 16, matrix->color565(0, 0, 0));
        }
        else
        {
            drawString("TIMES UP", 8, 4, 16, matrix->color565(255, 0, 0));
        }
    }
    matrix->flipDMABuffer();
}