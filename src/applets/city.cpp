#include "city.h"
#include "system.h"

static float posX = 0.0f;
static float moveStep = 4.0;
static float cTime = 0.0f;
static float timeScale = 100.0f;

static uint16_t col0 = matrix->color333(3, 6, 7);
static uint16_t col1 = matrix->color333(2, 3, 5);
static uint16_t col2 = matrix->color333(1, 1, 2);

static uint16_t sunCol = matrix->color333(7, 7, 7);
static uint16_t bgCol = matrix->color333(0, 0, 0);

void citySetup ()
{
    if (nightMode)
    {
        col0 = matrix->color333(7, 0, 0);
        col1 = matrix->color333(2, 3, 0);
        col2 = matrix->color333(4, 0, 0);
        sunCol = matrix->color333(0, 0, 0);
        bgCol = matrix->color333(6, 0, 2);
    } else {
        col0 = matrix->color333(0, 1, 2);
        col1 = matrix->color333(0, 3, 5);
        col2 = matrix->color333(0, 5, 5);
        sunCol = matrix->color333(7, 7, 7);
        bgCol = matrix->color333(2, 6, 6);
    }
}

void cityLoop ()
{
    matrix->fillScreen(bgCol);

    cTime = (float)millis() / 1000.0f * timeScale;

    while (rotationInput0 != 0)
    {
        if (rotationInput0 > 0)
        {
            timeScale += moveStep * 10;
            rotationInput0--;
        }
        else
        {
            timeScale -= moveStep * 10;
            rotationInput0++;
        }
    }

    matrix->fillCircle(matrix->width() / 2, matrix->height() / 2, 10, sunCol);

    matrix->drawBitmap( (int)(cTime * 0.25f) % 64 - 64, 0, pxbg_2, PXBG_0_WIDTH, PXBG_0_HEIGHT, col2);
    matrix->drawBitmap( (int)(cTime * 0.25f) % 64, 0, pxbg_2, PXBG_0_WIDTH, PXBG_0_HEIGHT, col2);

    matrix->drawBitmap( (int)(cTime * 0.5f) % 64 - 64, 0, pxbg_1, PXBG_0_WIDTH, PXBG_0_HEIGHT, col1);
    matrix->drawBitmap( (int)(cTime * 0.5f) % 64, 0, pxbg_1, PXBG_0_WIDTH, PXBG_0_HEIGHT, col1);

    matrix->drawBitmap( (int)(cTime * 1.0f) % 64 - 64, 0, pxbg_0, PXBG_0_WIDTH, PXBG_0_HEIGHT, col0);
    matrix->drawBitmap( (int)(cTime * 1.0f) % 64, 0, pxbg_0, PXBG_0_WIDTH, PXBG_0_HEIGHT, col0);

    //matrix->swapBuffers(true);
    matrix->flipDMABuffer();
}