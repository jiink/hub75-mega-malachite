#include "life.h"
#include "system.h"
#include "digits.h"

///////////////////////////////////////////////////////////////////////////////////////////
// ---- Definitions ---------------------------------------------------------------------//

#define FRAMEBUF_WIDTH 64
#define FRAMEBUF_HEIGHT 32
#define CELL_GRID_WIDTH 4
#define CELL_GRID_HEIGHT 2
#define MAX_PARTICLES_PER_CELL 100

#define MAX_PARTICLES 120
#define MAX_COLOR_GROUPS 8

#define RANDOMIZE_INTERVAL 15000
#define FPS_REPORT_INTERVAL 1000
#define WALLCLOCK_UPDATE_INTERVAL 1000
#define MONTH_REPORT_SIZE 2
#define DAY_REPORT_SIZE 2
#define HOUR_REPORT_SIZE 2

struct Vector2
{
    float x, y;
};

struct Vector2i
{
    int x, y;
};

enum ColorGroup
{
    GROUP_RED,
    GROUP_BLUE,
    GROUP_YELLOW
};

struct Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

Color ColorGroupColors[MAX_COLOR_GROUPS] = {
	{255, 20, 67},
	{20, 200, 255},
	{255, 200, 20},
	{255, 255, 255},
	{20, 255, 180},
	};

struct Particle
{
    Vector2 position;
    Vector2 velocity;
    ColorGroup colorGroup;
};

struct Cell
{
    uint16_t particleIndices[MAX_PARTICLES_PER_CELL];
    uint8_t particleCount;
};

// Used to define which way a neighboring cell is wrapped around the edge of the area
struct CellWrap
{
    bool wrappedLeft;
    bool wrappedRight;
    bool wrappedTop;
    bool wrappedBottom;
};

///////////////////////////////////////////////////////////////////////////////////////////
/* ---- Variables -----------------------------------------------------------------------*/

int wallclockUpdateTimer = 0;
int month, day, hour, minute, second, millisecond;

float attractionFactorMatrix[MAX_COLOR_GROUPS][MAX_COLOR_GROUPS];
Particle particles[MAX_PARTICLES];
int numColorGroups = 2;
// In worldspace, the radius of the sphere of influence for each particle.
const float maxDistance = 0.25; // Please let 2 be evenly divisible by this number, for cellSize's sake
const float dt = 0.01;
const float frictionFactor = 0.8;
const float forceFactor = 5.0;
float deltaTime = 0.0f;
int fps = 0;
int fpsTimer = 0;
int randomizeTimer = 0;
// Divide the area into cells whos size is the
// diameter of the circle of influence for every particle
const float cellSize = maxDistance * 2.0;
// Each grid cell contains a list of particles within its bounds.
Cell grid[CELL_GRID_HEIGHT][CELL_GRID_WIDTH];
Color FrameBuffer[FRAMEBUF_HEIGHT][FRAMEBUF_WIDTH];

///////////////////////////////////////////////////////////////////////////////////////////
/* ---- Math and help functions ---------------------------------------------------------*/

int positiveMod(int i, int n) {
    return (i % n + n) % n;
}

int clampI(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

// If there is an overflow, return 255
uint8_t addClamp(uint8_t a, uint8_t b)
{
	uint8_t sum = a + b;
	if (sum < a || sum < b)
	{
		return 255;
	}
	else
	{
		return sum;
	}
}

Vector2 vector2Subtract(Vector2 a, Vector2 b)
{
    return {a.x - b.x, a.y - b.y};
}

float vector2Length(Vector2 a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

Vector2 vector2Scale(Vector2 a, float scalar)
{
    return {a.x * scalar, a.y * scalar};
}

Vector2 vector2Add(Vector2 a, Vector2 b)
{
    return {a.x + b.x, a.y + b.y};
}

Vector2 vector2Normalize(Vector2 vec)
{
    float length = vector2Length(vec);
    if (length != 0.0f)
    {
        vec.x /= length;
        vec.y /= length;
    }
    return vec;
}

float attractionForceMag(float distance, float attractionFactor)
{
    // Closer than this, and the particles will push each other away
    const float tooCloseDistance = 0.4;
    if (distance < tooCloseDistance)
    {
        // Get away from me!
        return distance / tooCloseDistance - 1;
    }
    else if (tooCloseDistance < distance && distance < 1)
    {
        // Come closer
        return attractionFactor * (1.0 - abs(2.0 * distance - 1 - tooCloseDistance) / (1 - tooCloseDistance));
    }
    else
    {
        return 0.0;
    }
}

unsigned long timerDiff(unsigned long a, unsigned long b)
{
    if (a > b)
    {
        return a - b;
    }
    else
    {
        return b - a;
    }
}

float getAspectRatio()
{
    return (float)matrix->width() / (float)matrix->height();
}

float randFloat(float a, float b)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

uint8_t randByte(uint8_t a, uint8_t b)
{
    return rand() % (b - a + 1) + a;
}

static float squareIntersectionArea(Vector2 square1, Vector2 square2)
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

Color colorAdd(Color a, Color b)
{
	return {
		addClamp(a.r, b.r),
		addClamp(a.g, b.g),
		addClamp(a.b, b.b)};
}

Color colorMultiply(Color color, float value)
{
	return {
		(uint8_t)(color.r * value),
		(uint8_t)(color.g * value),
		(uint8_t)(color.b * value)};
}

Color hsvToRgb(float H, float S, float V) {
    if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0) {
        Color col = { 7, 7, 7 };
    }
    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
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
    int R = (r + m) * 255;
    int G = (g + m) * 255;
    int B = (b + m) * 255;
    return (Color) { R, G, B };
}

Color getRandomColor() {
    const float golden_ratio_conjugate = 0.618033988749895f * 360.0f;
    static float randomHue;
    randomHue += golden_ratio_conjugate;
    randomHue = fmod(randomHue, 360.0f);
    return hsvToRgb(randomHue, 100.0f, 100.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////
/* ---- Framebuffer functions -----------------------------------------------------------*/

void FrameBufferClear(Color color)
{
	for (int y = 0; y < FRAMEBUF_HEIGHT; y++)
	{
		for (int x = 0; x < FRAMEBUF_WIDTH; x++)
		{
			FrameBuffer[y][x] = color;
		}
	}
}

void frameBufferSetPix(int x, int y, Color color)
{
	// if (x < 0 || y < 0)
	// 	return;
	// if (x > FRAMEBUF_WIDTH - 1 || y > FRAMEBUF_HEIGHT - 1)
	// 	return;
    x = positiveMod(x, FRAMEBUF_WIDTH);
    y = positiveMod(y, FRAMEBUF_HEIGHT);
	FrameBuffer[y][x] = color;
}

void frameBufferSetPixV(Vector2 pos, Color color)
{
	frameBufferSetPix(pos.x, pos.y, color);
}

Color frameBufferGetPix(int x, int y)
{
	return FrameBuffer[y][x];
}

Color frameBufferGetPixV(Vector2 pos)
{
	return frameBufferGetPix(pos.x, pos.y);
}

void frameBufferAddPix(int x, int y, Color color)
{
	if (x < 0 || y < 0)
		return;
	if (x > FRAMEBUF_WIDTH - 1 || y > FRAMEBUF_HEIGHT - 1)
		return;
	frameBufferSetPix(x, y, colorAdd(frameBufferGetPix(x, y), color));
}

void frameBufferAddPixV(Vector2 pos, Color color)
{
	frameBufferAddPix(pos.x, pos.y, color);
}

void drawPixelVColor(Vector2 position, Color color)
{
    matrix->drawPixelRGB888((int)position.x, (int)position.y, color.r, color.g, color.b);
}

// Draws a point on the screen at a sub-pixel level, unlike DrawPixel.
// If the point is in-between screen pixels, it will be rendered using
// its neighboring pixels.
void drawPoint(Vector2 position, Color color)
{
	// Find the corners of the imaginary pixel-sized square around the point
	Vector2 cornerTopLeft = {position.x - 0.5f, position.y - 0.5f};

	// Find the corners of the squares of the grid pixels around the point
	Vector2 pixelCornerTopLeft = {floorf(position.x - 0.5f), floorf(position.y - 0.5f)};
	Vector2 pixelCornerTopRight = {pixelCornerTopLeft.x + 1.0f, pixelCornerTopLeft.y};
	Vector2 pixelCornerBottomLeft = {pixelCornerTopLeft.x, pixelCornerTopLeft.y + 1.0f};
	Vector2 pixelCornerBottomRight = {pixelCornerTopLeft.x + 1.0f, pixelCornerTopLeft.y + 1.0f};

	// Find the overlapping areas between the imaginary square around the point and
	// the grid squares
	float areaTopLeft = squareIntersectionArea(cornerTopLeft, pixelCornerTopLeft);
	float areaTopRight = squareIntersectionArea(cornerTopLeft, pixelCornerTopRight);
	float areaBottomLeft = squareIntersectionArea(cornerTopLeft, pixelCornerBottomLeft);
	float areaBottomRight = squareIntersectionArea(cornerTopLeft, pixelCornerBottomRight);

	// Find fractions of color
	Color colorTopLeft = colorMultiply(color, areaTopLeft);
	Color colorTopRight = colorMultiply(color, areaTopRight);
	Color colorBottomLeft = colorMultiply(color, areaBottomLeft);
	Color colorBottomRight = colorMultiply(color, areaBottomRight);

	// Set pixels
	frameBufferAddPixV(pixelCornerTopLeft, colorTopLeft);
	frameBufferAddPixV(pixelCornerTopRight, colorTopRight);
	frameBufferAddPixV(pixelCornerBottomLeft, colorBottomLeft);
	frameBufferAddPixV(pixelCornerBottomRight, colorBottomRight);
	// drawPixelVColor(pixelCornerTopLeft, colorTopLeft);
	// drawPixelVColor(pixelCornerTopRight, colorTopRight);
	// drawPixelVColor(pixelCornerBottomLeft, colorBottomLeft);
	// drawPixelVColor(pixelCornerBottomRight, colorBottomRight);
}

///////////////////////////////////////////////////////////////////////////////////////////
/* ---- Clock-related functions ---------------------------------------------------------*/

void wallclockUpdate()
{
    if (timerDiff(millis(), wallclockUpdateTimer) > WALLCLOCK_UPDATE_INTERVAL)
    {
        wallclockUpdateTimer = millis();
        time_t now;
        time(&now);
        tm* localTime = localtime(&now);
        month = localTime->tm_mon + 1;
        day = localTime->tm_mday;
        hour = localTime->tm_hour % 12;
        minute = localTime->tm_min;
        second = localTime->tm_sec;
        millisecond = 0;
    }
}

// Draws onto the framebuffer!!
void drawDigit(int digit, int x, int y, Color color)
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
                frameBufferAddPix(x + j, y + i, color);
            }
        }
    }
}

void drawNumberFb(int number, int x, int y, Color color)
{
    int digit1 = number / 10;
    int digit2 = number % 10;
    drawDigit(digit1, x, y, color);
    drawDigit(digit2, x + DIGIT_WIDTH + 1, y, color);
}

void drawProgressBarFb(int value, int maximum, Vector2i pos, int barMaxWidth, Color color)
{
    if (value > maximum)
        value = maximum;
    if (value < 0)
        value = 0;
    float barWidth = (float)value / (float)maximum * (float)barMaxWidth;
    int barWidthWhole = (int)barWidth;
    float barWidthFractional = barWidth - (float)barWidthWhole;
    for (int i = 0; i < barWidthWhole; i++)
    {
        frameBufferAddPix(pos.x + i, pos.y, color);
    }
    if (barWidthWhole < maximum && barWidthFractional > 0.0f)
    {
        frameBufferAddPix(pos.x + barWidthWhole, pos.y, colorMultiply(color, barWidthFractional));
    }
}

void drawClock()
{
    const Vector2i monthPos = { 1, 20 };
    const Vector2i dayPos = { 1, 26 };
    const Vector2i hourPos = { 56, 26 };
    const Vector2i hourBarPos = { 56, 24 };
    const Vector2i minuteBarPos = { 56, 22 };
    const Vector2i secondBarPos = { 56, 20 };
    const int barWidth = 7;
    const Color grey = { 127, 127, 127 };
    drawNumberFb(month, monthPos.x, monthPos.y, grey);
    drawNumberFb(day, dayPos.x, dayPos.y, grey);
    drawNumberFb(hour, hourPos.x, hourPos.y, grey);
    drawProgressBarFb(minute, 60, hourBarPos, barWidth, grey);
    drawProgressBarFb(second, 60, minuteBarPos, barWidth, grey);
    drawProgressBarFb(millis() % 1000, 1000, secondBarPos, barWidth, grey);
}

///////////////////////////////////////////////////////////////////////////////////////////
/* ---- Life-related functions ----------------------------------------------------------*/

void drawParticle(Particle* particle)
{
    Vector2 posOnScreen = {
        particle->position.x * matrix->width() / getAspectRatio(),
        particle->position.y * matrix->height()
        };
    drawPoint(posOnScreen, ColorGroupColors[particle->colorGroup]);
}

void drawParticles()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        drawParticle(&particles[i]);
    }
}

void initCellWraps(CellWrap *neighborCellWraps, uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        neighborCellWraps[i].wrappedLeft = false;
        neighborCellWraps[i].wrappedRight = false;
        neighborCellWraps[i].wrappedTop = false;
        neighborCellWraps[i].wrappedBottom = false;
    }
}

void randomizeAttractionFactorMatrix()
{
    for (int i = 0; i < MAX_COLOR_GROUPS; i++)
    {
        for (int j = 0; j < MAX_COLOR_GROUPS; j++)
        {
            attractionFactorMatrix[j][i] = randFloat(-1.0, 1.0);
        }
    }
}

void randomizeColorGroupColors()
{
    for (int i = 0; i < MAX_COLOR_GROUPS; i++)
    {
        ColorGroupColors[i] = getRandomColor();
    }
}

void randomizeColorGroups()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].colorGroup = (ColorGroup)randByte(0, numColorGroups - 1);
    }
}

void getNeighborCells(Cell **listToPopulate, int row, int col, CellWrap *wrapList)
{
    uint8_t left = (col == 0) ? CELL_GRID_WIDTH - 1 : col - 1;
    uint8_t right = (col + 1) % CELL_GRID_WIDTH;
    uint8_t above = (row == 0) ? CELL_GRID_HEIGHT - 1 : row - 1;
    uint8_t below = (row + 1) % CELL_GRID_HEIGHT;
    /*
    012
    345
    678
    */
    listToPopulate[0] = &grid[above][left];
    listToPopulate[1] = &grid[above][col];
    listToPopulate[2] = &grid[above][right];
    listToPopulate[3] = &grid[row][left];
    listToPopulate[4] = &grid[row][col];
    listToPopulate[5] = &grid[row][right];
    listToPopulate[6] = &grid[below][left];
    listToPopulate[7] = &grid[below][col];
    listToPopulate[8] = &grid[below][right];

    // Which way are the cells wrapped?
    if (col == 0)
    {
        // If the leftmost column, the left neighbors are all wrapped
        wrapList[0].wrappedLeft = true;
        wrapList[3].wrappedLeft = true;
        wrapList[6].wrappedLeft = true;
    }
    else if (col == CELL_GRID_WIDTH - 1)
    {
        // If the rightmost column, the right neighbors are all wrapped
        wrapList[2].wrappedRight = true;
        wrapList[5].wrappedRight = true;
        wrapList[8].wrappedRight = true;
    }
    if (row == 0)
    {
        wrapList[0].wrappedTop = true;
        wrapList[1].wrappedTop = true;
        wrapList[2].wrappedTop = true;
    }
    else if (row == CELL_GRID_HEIGHT - 1)
    {
        wrapList[6].wrappedBottom = true;
        wrapList[7].wrappedBottom = true;
        wrapList[8].wrappedBottom = true;
    }
}

void updateParticles()
{
    // Update each particle, one cell at a time
    for (int r = 0; r < CELL_GRID_HEIGHT; r++)
    {
        for (int c = 0; c < CELL_GRID_WIDTH; c++)
        {
            // Get list of the 8 neighboring cells and itself
            Cell *neighborCells[9];
            CellWrap neighborCellWraps[9];
            initCellWraps(neighborCellWraps, 9);
            getNeighborCells(neighborCells, r, c, neighborCellWraps);

            // Go through every particle in this cell (as subjects)
            for (int pI = 0; pI < grid[r][c].particleCount; pI++)
            {
                uint16_t i = grid[r][c].particleIndices[pI];
                Vector2 totalForce = {0.0, 0.0}; // Will be accumulated when looping through neighbors
                // Go through each neighboring cell
                for (int n = 0; n < 9; n++)
                {
                    // Go through every particle in this cell (as objects)
                    for (int pJ = 0; pJ < neighborCells[n]->particleCount; pJ++)
                    {
                        uint16_t j = neighborCells[n]->particleIndices[pJ];
                        if (&particles[j] == &particles[i])
                            continue;

                        Vector2 particleObjPercievedPos = particles[j].position;
                        // Offset location if it's wrapped
                        if (neighborCellWraps[n].wrappedLeft)
                        {
                            particleObjPercievedPos.x -= getAspectRatio();
                        }
                        if (neighborCellWraps[n].wrappedRight)
                        {
                            particleObjPercievedPos.x += getAspectRatio();
                        }
                        if (neighborCellWraps[n].wrappedTop)
                        {
                            particleObjPercievedPos.y -= 1.0;
                        }
                        if (neighborCellWraps[n].wrappedBottom)
                        {
                            particleObjPercievedPos.y += 1.0;
                        }

                        // Only deal with neighbors within sphere of influence
                        Vector2 delta = vector2Subtract(particles[i].position, particleObjPercievedPos);
                        float distance = vector2Length(delta);
                        if (distance > 0.001f && distance < maxDistance)
                        {
                            // How hard do I need to move?
                            float forceMag = attractionForceMag(distance / maxDistance, attractionFactorMatrix[particles[i].colorGroup][particles[j].colorGroup]);

                            // Where do I need to move?
                            // Normalize then scale by force magnitude
                            Vector2 force = vector2Scale(delta, -1.0f / distance * forceMag);
                            totalForce = vector2Add(totalForce, force);
                        }
                    }
                }

                totalForce = vector2Scale(totalForce, maxDistance * forceFactor);

                particles[i].velocity = vector2Scale(particles[i].velocity, frictionFactor);
                particles[i].velocity = vector2Add(particles[i].velocity, vector2Scale(totalForce, deltaTime));

                // Update the particle's position based on its velocity
                particles[i].position.x += particles[i].velocity.x * deltaTime;
                particles[i].position.y += particles[i].velocity.y * deltaTime;

                // If the particle goes off the screen, wrap it around to the other side
                if (particles[i].position.x < 0.01)
                    particles[i].position.x = 1.99;
                if (particles[i].position.x > 2)
                    particles[i].position.x = 0.01;
                if (particles[i].position.y < 0.01)
                    particles[i].position.y = 0.99;
                if (particles[i].position.y > 1)
                    particles[i].position.y = 0.01;
            }
        }
    }
}

void updateGrid()
{
    // Clear the list of particles for each cell
    for (int i = 0; i < CELL_GRID_HEIGHT; i++)
    {
        for (int j = 0; j < CELL_GRID_WIDTH; j++)
        {
            grid[i][j].particleCount = 0;
        }
    }

    // Add each particle to the list of particles for its corresponding cell
    for (int i = 0; i < MAX_PARTICLES; i++)
    {

        int cell_row = particles[i].position.y / cellSize;
        int cell_col = particles[i].position.x / cellSize;
        
        Cell *cell = &grid[cell_row][cell_col];
        if (cell->particleCount < MAX_PARTICLES_PER_CELL)
        {
            cell->particleIndices[cell->particleCount] = i;
            cell->particleCount++;
        }
    }

}

void lifeUpdate()
{
    // Update time
    static unsigned long prevMillis = 0;
    unsigned long currentMillis = millis();
    deltaTime = (currentMillis - prevMillis) / 1000.0f;
    //deltaTime = 1.0f / 60.0f;
    prevMillis = currentMillis;
    if (timerDiff(currentMillis, fpsTimer) > FPS_REPORT_INTERVAL)
    {
        fpsTimer = currentMillis;
        fps = 1.0f / deltaTime;
    }
    if (timerDiff(currentMillis, randomizeTimer) > RANDOMIZE_INTERVAL)
    {
        randomizeTimer = currentMillis;
        randomizeAttractionFactorMatrix();
        randomizeColorGroupColors();
    }
    while (rotationInput0 != 0)
    {
        if (rotationInput0 > 0)
        {
            numColorGroups = clampI(numColorGroups + 1, 1, MAX_COLOR_GROUPS);
            rotationInput0--;
        }
        else
        {
            numColorGroups = clampI(numColorGroups - 1, 1, MAX_COLOR_GROUPS);
            rotationInput0++;
        }
        randomizeColorGroups();
        randomizeAttractionFactorMatrix();
    }
    updateGrid();
    updateParticles();
}

void drawFrameBuffer()
{
    for (int y = 0; y < FRAMEBUF_HEIGHT; y++)
    {
        for (int x = 0; x < FRAMEBUF_WIDTH; x++)
        {
            matrix->drawPixelRGB888(x, y, FrameBuffer[y][x].r, FrameBuffer[y][x].g, FrameBuffer[y][x].b);
        }
    }
}

void lifeDraw()
{
    //matrix->clearScreen();
    FrameBufferClear((Color){0, 0, 0});
    drawClock();
    drawParticles();
    drawFrameBuffer();
    matrix->flipDMABuffer();
}

///////////////////////////////////////////////////////////////////////////////////////////
/* ---- Setup and Loop ------------------------------------------------------------------*/

void lifeSetup()
{
    // Initialize the particles with random positions, velocities, and colors
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].position = {randFloat(0, getAspectRatio()), randFloat(0, 1)};
        //particles[i].position = {(float)i/32, (float)i/32};
        //particles[i].velocity = {randFloat(-1, 1), randFloat(-1, 1)};
        particles[i].velocity = {randFloat(-1, 1), randFloat(-1, 1)};
        //particles[i].velocity = {1.0f, -1.0};
        particles[i].colorGroup = (ColorGroup)randByte(GROUP_RED, MAX_COLOR_GROUPS - 1);
    }

    attractionFactorMatrix[0][0] = 1.0;
    attractionFactorMatrix[0][1] = -1.0;
    attractionFactorMatrix[1][0] = 0.2;
    attractionFactorMatrix[1][1] = 0.0;
}

void lifeLoop()
{
    wallclockUpdate();
    lifeUpdate();
    lifeDraw();
}