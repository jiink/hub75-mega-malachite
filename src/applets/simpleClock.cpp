#include "system.h"
#include "simpleClock.h"
#include "digits.h"
#include "letters.h"

// 2-letter abbreviations
static const uint8_t MONTH_LETTERS[12][2] = {
    {'J', 'A'}, // January
    {'F', 'E'}, // February
    {'M', 'A'}, // March
    {'A', 'P'}, // April
    {'M', 'Y'}, // May
    {'J', 'U'}, // June
    {'J', 'L'}, // July
    {'A', 'U'}, // August
    {'S', 'P'}, // September
    {'O', 'C'}, // October
    {'N', 'V'}, // November
    {'D', 'C'}  // December
};

struct Vector2i
{
    int x, y;
};

struct TimeInfo {
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

TimeInfo currentTime = {0, 0, 0, 0, 0};
bool dotsVisibile = true;
float scVal = 100.0f;

static unsigned long timerDiff(unsigned long a, unsigned long b)
{
    if (a > b) { return a - b; }
    else { return b - a; }
}

static void wallclockUpdate(TimeInfo& ct)
{
    const unsigned long WALLCLOCK_UPDATE_INTERVAL = 10000;
    static unsigned long wallclockUpdateTimer = 0;
    if (timerDiff(millis(), wallclockUpdateTimer) > WALLCLOCK_UPDATE_INTERVAL)
    {
        wallclockUpdateTimer = millis();
        time_t now;
        time(&now);
        tm* localTime = localtime(&now);
        ct.month = localTime->tm_mon + 1;
        ct.day = localTime->tm_mday;
        ct.hour = localTime->tm_hour % 12;
        ct.minute = localTime->tm_min;
        ct.second = localTime->tm_sec;

        dotsVisibile = !dotsVisibile;
    }
}

static void monthNumTo2Letters(int monthNum, uint8_t& letter1, uint8_t& letter2)
{
    if (monthNum < 1 || monthNum > 12)
    {
        letter1 = 0;
        letter2 = 0;
    }
    else
    {
        letter1 = MONTH_LETTERS[monthNum - 1][0];
        letter2 = MONTH_LETTERS[monthNum - 1][1];
    }
}

static void draw2LetterMonth(int monthNum, int x, int y, uint16_t color)
{
    uint8_t letter1 = 0;
    uint8_t letter2 = 0;
    monthNumTo2Letters(monthNum, letter1, letter2);
    drawLetter(letter1, x, y, color);
    drawLetter(letter2, x + LETTER_WIDTH + 1, y, color);
}

uint16_t hsvToRgb565(float H, float S, float V) {
    // Wrap H to [0.0, 1.0)
    H = fmodf(H, 1.0f);
    if (H < 0.0f) H += 1.0f;

    // Scale H to [0, 360)
    H *= 360.0f;

    if (S > 100 || S < 0 || V > 100 || V < 0) {
        return 0xFFFF; // default to white if out of range
    }
    float s = S / 100.0f;
    float v = V / 100.0f;
    float C = s * v;
    float X = C * (1 - fabsf(fmodf(H / 60.0f, 2.0f) - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60) {
        r = C, g = X, b = 0;
    }
    else if (H >= 60 && H < 120) {
        r = X, g = C, b = 0;
    }
    else if (H >= 120 && H < 180) {
        r = 0, g = C, b = X;
    }
    else if (H >= 180 && H < 240) {
        r = 0, g = X, b = C;
    }
    else if (H >= 240 && H < 300) {
        r = X, g = 0, b = C;
    }
    else {
        r = C, g = 0, b = X;
    }
    int R = (int)((r + m) * 255);
    int G = (int)((g + m) * 255);
    int B = (int)((b + m) * 255);

    // Convert to RGB565
    uint16_t rgb565 = ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3);
    return rgb565;
}

static void drawClock()
{
    const Vector2i hourPos = { 24, 12 };
    const Vector2i minutePos = { 34, 12 };
    const Vector2i monthPos = { 24, 25 };
    const Vector2i dayPos = { 34, 25 };
    const Vector2i dotsPos = { 32, 13 };
    float hue = (currentTime.hour - 1) / 11.0f;
    uint16_t col = hsvToRgb565(hue, 100.0f, scVal);
    draw2DigitNumber(currentTime.hour, hourPos.x, hourPos.y, col);
    draw2DigitNumber(currentTime.minute, minutePos.x, minutePos.y, col);
    draw2LetterMonth(currentTime.month, monthPos.x, monthPos.y, col);
    draw2DigitNumber(currentTime.day, dayPos.x, dayPos.y, col);
    matrix->drawPixel(dotsPos.x, dotsPos.y, col);
    matrix->drawPixel(dotsPos.x, dotsPos.y + 2, col);
}

void simpleClockSetup()
{
    if (nightMode)
    {
        scVal = 50.0f;
    } else {
        scVal = 100.0f;
    }
}

void simpleClockLoop()
{
    // Update ----------------------
    wallclockUpdate(currentTime);

    // Draw ------------------------
    matrix->fillScreen(matrix->color565(0, 0, 0));
    drawClock();
    matrix->flipDMABuffer();
}