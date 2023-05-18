#include <stdint.h>
#include <math.h>
#include "system.h"
#include "movingPixel.h"

uint16_t movingPixelColor = matrix.Color333(7, 7, 7);

float direction = 0.0f;
float speed = 1.0f;
float xVelocity = 0.0f;
float yVelocity = 0.0f;
float x = 0.0f;
float y = 0.0f;

void movingPixelSetup(void)
{
	movingPixelColor = matrix.Color333(7, 6, 0);
}

void updateDirection(){
	const float directionIncrement = 0.1f;
	while (rotationInput != 0)
	{
		if (rotationInput > 0)
		{
			direction += directionIncrement;
			rotationInput--;
		}
		else
		{
			direction -= directionIncrement;
			rotationInput++;
		}
	}
	xVelocity = speed * cos(direction);
	yVelocity = speed * sin(direction);

	x += xVelocity;
	y += yVelocity;

	// Wrap around
	if (x < 0.0f)
	{
		x = (float)matrix.width();
	}
	else if (x > (float)matrix.width())
	{
		x = 0.0f;
	}
	if (y < 0.0f)
	{
		y = (float)matrix.height();
	}
	else if (y > (float)matrix.height())
	{
		y = 0.0f;
	}
}

void movingPixelLoop(void)
{
	matrix.fillScreen(matrix.Color333(0, 0, 0));

	updateDirection();

	matrix.drawPixel(x, y, movingPixelColor);

	matrix.swapBuffers(true);
}
