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

const uint8_t PanelColorDepth = 3; // Per channel
struct PanelColor
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct Vector2
{
	float x, y;
};

float SquareIntersectionArea(Vector2 square1, Vector2 square2)
{
	float left = fmax(square1.x, square2.x);
	float right = fmin(square1.x + 1, square2.x + 1);
	float top = fmax(square1.y, square2.y);
	float bottom = fmin(square1.y + 1, square2.y + 1);

	float width = right - left;
	float height = bottom - top;

	if (width <= 0 || height <= 0)
	{
		return 0;
	}
	else
	{
		return width * height;
	}
}

PanelColor PanelColorMultiply(PanelColor color, float value)
{
	return {
		(uint8_t)(color.r * value),
		(uint8_t)(color.g * value),
		(uint8_t)(color.b * value)};
}

uint16_t PanelColor333(PanelColor panelColor)
{
	// uint8_t mask = (1 << (8 - PanelColorDepth)) - 1;
	return matrix.Color333(panelColor.r / 32, panelColor.g / 32, panelColor.b / 32);
}

// Draws a point on the screen at a sub-pixel level, unlike DrawPixel.
// If the point is in-between screen pixels, it will be rendered using
// its neighboring pixels.
void DrawPoint(Vector2 position, PanelColor color)
{
	// Find the corners of the imaginary pixel-sized square around the point
	Vector2 cornerTopLeft = {position.x - 0.5f, position.y - 0.5f};

	// Find the corners of the squares of the grid pixels around the point
	Vector2 pixelCornerTopLeft = {floorf(position.x - 0.5), floorf(position.y - 0.5)};
	Vector2 pixelCornerTopRight = {pixelCornerTopLeft.x + 1.0, pixelCornerTopLeft.y};
	Vector2 pixelCornerBottomLeft = {pixelCornerTopLeft.x, pixelCornerTopLeft.y + 1.0};
	Vector2 pixelCornerBottomRight = {pixelCornerTopLeft.x + 1.0, pixelCornerTopLeft.y + 1.0};

	// Find the overlapping areas between the imaginary square around the point and
	// the grid squares
	float areaTopLeft = SquareIntersectionArea(cornerTopLeft, pixelCornerTopLeft);
	float areaTopRight = SquareIntersectionArea(cornerTopLeft, pixelCornerTopRight);
	float areaBottomLeft = SquareIntersectionArea(cornerTopLeft, pixelCornerBottomLeft);
	float areaBottomRight = SquareIntersectionArea(cornerTopLeft, pixelCornerBottomRight);

	// Find fractions of color
	PanelColor colorTopLeft = PanelColorMultiply(color, areaTopLeft);
	PanelColor colorTopRight = PanelColorMultiply(color, areaTopRight);
	PanelColor colorBottomLeft = PanelColorMultiply(color, areaBottomLeft);
	PanelColor colorBottomRight = PanelColorMultiply(color, areaBottomRight);

	// Set pixels
	// FrameBufferAddPixV(pixelCornerTopLeft, colorTopLeft);
	// FrameBufferAddPixV(pixelCornerTopRight, colorTopRight);
	// FrameBufferAddPixV(pixelCornerBottomLeft, colorBottomLeft);
	// FrameBufferAddPixV(pixelCornerBottomRight, colorBottomRight);
	matrix.drawPixel(pixelCornerTopLeft.x, pixelCornerTopLeft.y, PanelColor333(colorTopLeft));
	matrix.drawPixel(pixelCornerTopRight.x, pixelCornerTopRight.y, PanelColor333(colorTopRight));
	matrix.drawPixel(pixelCornerBottomLeft.x, pixelCornerBottomLeft.y, PanelColor333(colorBottomLeft));
	matrix.drawPixel(pixelCornerBottomRight.x, pixelCornerBottomRight.y, PanelColor333(colorBottomRight));
}


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

	DrawPoint({x, y}, {255, 255, 0});

	matrix.swapBuffers(true);
}
