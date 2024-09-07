#include "system.h"
#include "digits.h"

void drawDigit(int digit, int x, int y, uint16_t color)
{
    const uint8_t* digitData;
    if (digit < 0 || digit > 9)
    {
        digitData = DIGIT_0;
    }
    else
    {
        digitData = DIGITS[digit];
    }
    for (int i = 0; i < DIGIT_HEIGHT; i++)
    {
        for (int j = 0; j < DIGIT_WIDTH; j++)
        {
            uint8_t rowBits = digitData[i];
            if (rowBits & (1 << (DIGIT_WIDTH - 1 - j)))  // Read the bits in reverse order
            {
                matrix->drawPixel(x + j, y + i, color);
            }
        }
    }
}

void draw2DigitNumber(int number, int x, int y, uint16_t color)
{
    int digit1 = number / 10;
    int digit2 = number % 10;
    drawDigit(digit1, x, y, color);
    drawDigit(digit2, x + DIGIT_WIDTH + 1, y, color);
}

void drawDigitBig(int digit, int x, int y, uint16_t color)
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
    else if (digit == 11)
    {
        digitData = DIGITBIG_COLON;
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

void draw2DigitBigNumber(int number, int x, int y, uint16_t color)
{
    int digit1 = number / 10;
    int digit2 = number % 10;
    drawDigitBig(digit1, x, y, color);
    drawDigitBig(digit2, x + DIGITBIG_WIDTH + 1, y, color);
}
