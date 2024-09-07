#include <stdint.h>
#include <pgmspace.h>
#ifndef LETTERS_H
#define LETTERS_H

#define NUM_LETTERS 26
#define LETTER_WIDTH  3
#define LETTER_HEIGHT 5

static const uint8_t LETTER_A[] PROGMEM  = {
    0b111,
    0b101,
    0b111,
    0b101,
    0b101
};
static const uint8_t LETTER_B[] PROGMEM  = {
    0b110,
    0b101,
    0b110,
    0b101,
    0b110
};
static const uint8_t LETTER_C[] PROGMEM  = {
    0b111,
    0b100,
    0b100,
    0b100,
    0b111
};
static const uint8_t LETTER_D[] PROGMEM  = {
    0b110,
    0b101,
    0b101,
    0b101,
    0b110
};
static const uint8_t LETTER_E[] PROGMEM  = {
    0b111,
    0b100,
    0b111,
    0b100,
    0b111
};
static const uint8_t LETTER_F[] PROGMEM  = {
    0b111,
    0b100,
    0b111,
    0b100,
    0b100
};
static const uint8_t LETTER_G[] PROGMEM  = {
    0b111,
    0b100,
    0b101,
    0b101,
    0b111
};
static const uint8_t LETTER_H[] PROGMEM  = {
    0b101,
    0b101,
    0b111,
    0b101,
    0b101
};
static const uint8_t LETTER_I[] PROGMEM  = {
    0b111,
    0b010,
    0b010,
    0b010,
    0b111
};
static const uint8_t LETTER_J[] PROGMEM  = {
    0b111,
    0b001,
    0b001,
    0b101,
    0b010
};
static const uint8_t LETTER_K[] PROGMEM  = {
    0b101,
    0b101,
    0b110,
    0b101,
    0b101
};
static const uint8_t LETTER_L[] PROGMEM  = {
    0b100,
    0b100,
    0b100,
    0b100,
    0b111
};
static const uint8_t LETTER_M[] PROGMEM  = {
    0b101,
    0b111,
    0b101,
    0b101,
    0b101
};
static const uint8_t LETTER_N[] PROGMEM  = {
    0b101,
    0b111,
    0b111,
    0b111,
    0b101
};
static const uint8_t LETTER_O[] PROGMEM  = {
    0b111,
    0b101,
    0b101,
    0b101,
    0b111
};
static const uint8_t LETTER_P[] PROGMEM  = {
    0b111,
    0b101,
    0b111,
    0b100,
    0b100
};
static const uint8_t LETTER_Q[] PROGMEM  = {
    0b111,
    0b101,
    0b101,
    0b111,
    0b001
};
static const uint8_t LETTER_R[] PROGMEM  = {
    0b110,
    0b101,
    0b111,
    0b110,
    0b101
};
static const uint8_t LETTER_S[] PROGMEM  = {
    0b011,
    0b100,
    0b111,
    0b001,
    0b110
};
static const uint8_t LETTER_T[] PROGMEM  = {
    0b111,
    0b010,
    0b010,
    0b010,
    0b010
};
static const uint8_t LETTER_U[] PROGMEM  = {
    0b101,
    0b101,
    0b101,
    0b101,
    0b111
};
static const uint8_t LETTER_V[] PROGMEM  = {
    0b101,
    0b101,
    0b101,
    0b101,
    0b010
};
static const uint8_t LETTER_W[] PROGMEM  = {
    0b101,
    0b101,
    0b101,
    0b111,
    0b101
};
static const uint8_t LETTER_X[] PROGMEM  = {
    0b101,
    0b101,
    0b010,
    0b101,
    0b101
};
static const uint8_t LETTER_Y[] PROGMEM  = {
    0b101,
    0b101,
    0b111,
    0b001,
    0b111
};
static const uint8_t LETTER_Z[] PROGMEM  = {
    0b111,
    0b001,
    0b010,
    0b100,
    0b111
};

const uint8_t* const LETTERS[] PROGMEM = {
    LETTER_A,
    LETTER_B,
    LETTER_C,
    LETTER_D,
    LETTER_E,
    LETTER_F,
    LETTER_G,
    LETTER_H,
    LETTER_I,
    LETTER_J,
    LETTER_K,
    LETTER_L,
    LETTER_M,
    LETTER_N,
    LETTER_O,
    LETTER_P,
    LETTER_Q,
    LETTER_R,
    LETTER_S,
    LETTER_T,
    LETTER_U,
    LETTER_V,
    LETTER_W,
    LETTER_X,
    LETTER_Y,
    LETTER_Z
};

void drawLetter(char letter, int x, int y, uint16_t color);
void drawString(const char* str, size_t len, int x, int y, uint16_t color);

#endif