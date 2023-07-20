#include <math.h>
#include "system.h"

float pTime = 0.0f;
float timeScale = 0.2f;

int moonSize = 12; //Radius of moon in pixels
uint16_t moonColor = matrix->color333(2, 5, 7);
uint16_t bgColor = matrix->color333(0, 0, 0);

int midX;
int midY;

// draw an elipse from centered at X,Y with width and height as specified
void fillEllipse (int originX, int originY, uint32_t width, uint32_t height, uint16_t color)
{
    long long hh = height * height;
    long long ww = width * width;
    long long hhww = hh * ww;
    long long x0 = width;
    long long dx = 0;

    // do the horizontal diameter
    for (long long x = -width; x <= width; x++)
        matrix->drawPixel(originX + x, originY, color);

    // now do both halves at the same time, away from the diameter
    for (long long y = 1; y <= height; y++)
    {
        long long x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
        for ( ; x1 > 0; x1--)
            if (x1*x1*hh + y*y*ww <= hhww)
                break;
        dx = x0 - x1;  // current approximation of the slope
        x0 = x1;

        for (long long x = -x0; x <= x0; x++)
        {
            matrix->drawPixel(originX + x, originY - y, color);
            matrix->drawPixel(originX + x, originY + y, color);
        }
    }
}

// draw an elipse from centered at X,Y with width and height as specified
void drawEllipse (int x, int y, uint8_t width, uint8_t height, uint8_t on)
{
    long x1 = -width, y1 = 0; // II quadrant from bottom left to top right
    long e2 = height, dx = (1 + 2 * x1) * e2 * e2; // error increment
    long dy = x1 * x1, err = dx + dy; // error of 1 step

    do {
        matrix->drawPixel(x - x1, y + y1, on); // I Quadrant
        matrix->drawPixel(x + x1, y + y1, on); // II Quadrant
        matrix->drawPixel(x + x1, y - y1, on); // III Quadrant
        matrix->drawPixel(x - x1, y - y1, on); // IV Quadrant
        e2 = 2 * err;

        if (e2 >= dx) {
            x1++;
            err += dx += 2 * (long) height * height;
        } // x1 step

        if (e2 <= dy) {
            y1++;
            err += dy += 2 * (long) width * width;
        } // y1 step
    } while (x1 <= 0);

    while (y1++ < height) { // too early stop for flat ellipses with width=1
        matrix->drawPixel(x, y + y1, on); // -> finish tip of ellipse
        matrix->drawPixel(x, y - y1, on);
    }
}


void phasesSetup()
{
    midX = matrix->width() / 2;
    midY = matrix->height() / 2;
}


void phasesLoop()
{
    matrix->fillScreen(matrix->color333(0, 0, 0));
    pTime = (float)millis() / 1000.0f * timeScale;

    matrix->fillCircle(midX, midY, moonSize, moonColor);

    uint16_t centerColor;
    if (((int)(pTime - 0.5) % 2))
    {
        centerColor = bgColor;
    }
    else
    {
        centerColor = moonColor;
    }

    matrix->fillRect(
        ((int)pTime % 2) * moonSize + (midX - moonSize),
        midY - moonSize,
        moonSize + 1,
        2 * moonSize + 1,
        bgColor
    );


    fillEllipse(midX, midY, moonSize * abs(cos(PI * pTime)) + 1, moonSize + 1, centerColor);
    

    //matrix->swapBuffers(true);
    matrix->flipDMABuffer();
}