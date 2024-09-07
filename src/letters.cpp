#include "system.h"
#include "letters.h"

void drawLetter(char letter, int x, int y, uint16_t color)
{
    if (' ' == letter)
    {
        return;
    }
    int letterIdx = letter - 'A';
    const uint8_t* letterData;
    if (letterIdx < 0 || letterIdx >= NUM_LETTERS)
    {
        letterData = LETTER_X;
    }
    else
    {
        letterData = LETTERS[letterIdx];
    }
    for (int i = 0; i < LETTER_HEIGHT; i++)
    {
        for (int j = 0; j < LETTER_WIDTH; j++)
        {
            uint8_t rowBits = letterData[i];
            if (rowBits & (1 << (LETTER_WIDTH - 1 - j)))  // Read the bits in reverse order
            {
                matrix->drawPixel(x + j, y + i, color);
            }
        }
    }
}

void drawString(const char* str, size_t len, int x, int y, uint16_t color)
{
    for (int i = 0; i < len; i++)
    {
        drawLetter(str[i], x + i * (LETTER_WIDTH + 1), y, color);
    }
}
