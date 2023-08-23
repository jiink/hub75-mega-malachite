#include "life.h"
#include "system.h"

///////////////////////////////////////////////////////////////////////////////////////////
// ---- Definitions ---------------------------------------------------------------------//

#define CELL_GRID_WIDTH 4
#define CELL_GRID_HEIGHT 2
#define MAX_PARTICLES_PER_CELL 500

#define MAX_PARTICLES 32
#define MAX_COLOR_GROUPS 2

#define FPS_REPORT_INTERVAL 1000

struct Vector2
{
	float x, y;
};

enum ColorGroup
{
    GROUP_RED,
    GROUP_BLUE,
    GROUP_YELLOW
};

// RGB 565
const uint16_t ColorGroupColors[] = {
    0xf980,
    0x47df,
    0xee21,
    0xffff,
    0x12fd};

struct Particle
{
    Vector2 position;
    Vector2 velocity;
    ColorGroup colorGroup;
};

///////////////////////////////////////////////////////////////////////////////////////////
// ---- Variables -----------------------------------------------------------------------//

float attractionFactorMatrix[MAX_COLOR_GROUPS][MAX_COLOR_GROUPS];
Particle particles[MAX_PARTICLES];
// In worldspace, the radius of the sphere of influence for each particle.
const float maxDistance = 0.25; // Please let 2 be evenly divisible by this number, for cellSize's sake
const float dt = 0.01;
const float frictionFactor = 0.8;
const float forceFactor = 5.0;
float deltaTime = 0.0f;
int fps = 0;
int fpsTimer = 0;

///////////////////////////////////////////////////////////////////////////////////////////
// ---- Functions -----------------------------------------------------------------------//

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

float randomFloat() {
    return (float)rand() / RAND_MAX;
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

void drawParticle(Particle* particle)
{
    matrix->drawPixel(
        particle->position.x * matrix->width() / getAspectRatio(),
        particle->position.y * matrix->height(),
        ColorGroupColors[particle->colorGroup]
    );
}

void drawParticles()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        drawParticle(&particles[i]);
    }
}

void updateParticles()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        Particle* particle = &particles[i];
        // Update the particle's position based on its velocity
        particle->position.x += particle->velocity.x * deltaTime;
        particle->position.y += particle->velocity.y * deltaTime;

        // If the particle goes off the screen, wrap it around to the other side
        // if (particle->position.x < 0.01f)
        //     particle->position.x = 1.99f;
        // if (particle->position.x > 2.0f)
        //     particle->position.x = 0.01f;
        // if (particle->position.y < 0.01f)
        //     particle->position.y = 0.99f;
        // if (particle->position.y > 1.0f)
        //     particle->position.y = 0.01f;

        // Contain the particle in the screen
        if (particle->position.x < 0.0f)
        {
            particle->position.x = 0.0f;
            particle->velocity.x *= -1.0f;
        }
        if (particle->position.x > getAspectRatio())
        {
            particle->position.x = getAspectRatio();
            particle->velocity.x *= -1.0f;
        }
        if (particle->position.y < 0.0f)
        {
            particle->position.y = 0.0f;
            particle->velocity.y *= -1.0f;
        }
        if (particle->position.y > 1.0f)
        {
            particle->position.y = 1.0f;
            particle->velocity.y *= -1.0f;
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
    updateParticles();
}

void lifeDraw()
{
    matrix->fillScreen(matrix->color565(0, 128, 50));
    matrix->setTextColor(matrix->color565(0, 255, 128));
	matrix->setCursor(0, 0);
    matrix->printf("%d", fps);
    drawParticles();
    //matrix->printf("%.2f", particles[MAX_PARTICLES - 1].position.x);
    matrix->flipDMABuffer();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ---- Setup and Loop ------------------------------------------------------------------//

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
    lifeUpdate();
    lifeDraw();
}