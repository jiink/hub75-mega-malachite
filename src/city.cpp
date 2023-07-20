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
    
}

void cityLoop ()
{
    matrix->fillScreen(bgCol);

    cTime = (float)millis() / 1000.0f * timeScale;

    while (rotationInput != 0)
	{
		if (rotationInput > 0)
		{
			posX += moveStep;
			rotationInput--;
		}
		else
		{
			posX -= moveStep;
			rotationInput++;
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