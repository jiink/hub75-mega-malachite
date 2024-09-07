#include "timer.h"
#include "system.h"
#include "digits.h"

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
    matrix->fillScreen(matrix->color565(0, 0, 0));
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

    draw2DigitBigNumber(hours, 0, 0, digitColor);
    drawDigitBig(10, 16, 0, digitColor);
    draw2DigitBigNumber(minutes, 24, 0, digitColor);
    drawDigitBig(10, 40, 0, digitColor);
    draw2DigitBigNumber(seconds, 48, 0, digitColor);
}