#include <stdint.h>
#include <pgmspace.h>
#ifndef DIGITS_H
#define DIGITS_H

#define DIGIT_WIDTH  3
#define DIGIT_HEIGHT 5

static const uint8_t DIGIT_0[] PROGMEM  = {
    0b111,
    0b101,
    0b101,
    0b101,
    0b111
};
static const uint8_t DIGIT_1[] PROGMEM  = {
    0b010,
    0b110,
    0b010,
    0b010,
    0b111
};
static const uint8_t DIGIT_2[] PROGMEM  = {
    0b111,
    0b001,
    0b111,
    0b100,
    0b111
};
static const uint8_t DIGIT_3[] PROGMEM  = {
    0b111,
    0b001,
    0b111,
    0b001,
    0b111
};
static const uint8_t DIGIT_4[] PROGMEM  = {
    0b101,
    0b101,
    0b111,
    0b001,
    0b001
};
static const uint8_t DIGIT_5[] PROGMEM  = {
    0b111,
    0b100,
    0b111,
    0b001,
    0b111
};
static const uint8_t DIGIT_6[] PROGMEM  = {
    0b111,
    0b100,
    0b111,
    0b101,
    0b111
};
static const uint8_t DIGIT_7[] PROGMEM  = {
    0b111,
    0b001,
    0b001,
    0b001,
    0b001
};
static const uint8_t DIGIT_8[] PROGMEM  = {
    0b111,
    0b101,
    0b111,
    0b101,
    0b111
};
static const uint8_t DIGIT_9[] PROGMEM  = {
    0b111,
    0b101,
    0b111,
    0b001,
    0b111
};
const uint8_t* const DIGITS[] PROGMEM = {
    DIGIT_0,
    DIGIT_1,
    DIGIT_2,
    DIGIT_3,
    DIGIT_4,
    DIGIT_5,
    DIGIT_6,
    DIGIT_7,
    DIGIT_8,
    DIGIT_9
};

#define DIGITBIG_WIDTH  8
#define DIGITBIG_HEIGHT 10

static const uint8_t DIGITBIG_0[] PROGMEM  = {
    0b00111100,
    0b01111110,
    0b01100110,
    0b01100110,
    0b01100110,
    0b01100110,
    0b01100110,
    0b01100110,
    0b01111110,
    0b00111100,
};
static const uint8_t DIGITBIG_1[] PROGMEM  = {
    0b00011000,
    0b00111000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
};
static const uint8_t DIGITBIG_2[] PROGMEM  = {
    0b00111100,
    0b01111110,
    0b01100110,
    0b00000110,
    0b00001100,
    0b00011000,
    0b00110000,
    0b01100000,
    0b01111110,
    0b01111110,
};
static const uint8_t DIGITBIG_3[] PROGMEM  = {
    0b00111100,
    0b01111110,
    0b01100110,
    0b00000110,
    0b00011100,
    0b00011100,
    0b00000110,
    0b01100110,
    0b01111110,
    0b00111100,
};
static const uint8_t DIGITBIG_4[] PROGMEM  = {
    0b00001100,
    0b00011100,
    0b00011100,
    0b00111100,
    0b00101100,
    0b01101100,
    0b01111110,
    0b01111110,
    0b00001100,
    0b00001100,
};
static const uint8_t DIGITBIG_5[] PROGMEM  = {
    0b01111110,
    0b01111110,
    0b01100000,
    0b01100000,
    0b01111100,
    0b01111110,
    0b00000110,
    0b01100110,
    0b01111110,
    0b00111100,
};
static const uint8_t DIGITBIG_6[] PROGMEM  = {
    0b00111100,
    0b01111110,
    0b01100110,
    0b01100000,
    0b01111100,
    0b01111110,
    0b01100110,
    0b01100110,
    0b01111110,
    0b00111100,
};
static const uint8_t DIGITBIG_7[] PROGMEM  = {
    0b01111110,
    0b01111110,
    0b00000110,
    0b00001100,
    0b00001100,
    0b00001100,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
};
static const uint8_t DIGITBIG_8[] PROGMEM  = {
    0b00111100,
    0b01111110,
    0b01100110,
    0b01100110,
    0b00111100,
    0b01111110,
    0b01100110,
    0b01100110,
    0b01111110,
    0b00111100,
};
static const uint8_t DIGITBIG_9[] PROGMEM  = {
    0b00111100,
    0b01111110,
    0b01100110,
    0b01100110,
    0b01111110,
    0b00111110,
    0b00000110,
    0b01100110,
    0b01111110,
    0b00111100,
};
static const uint8_t DIGITBIG_DEGREE[] PROGMEM  = {
    0b01110000,
    0b11111000,
    0b11011000,
    0b11011000,
    0b11111000,
    0b01110000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
};

const uint8_t* const DIGITSBIG[] PROGMEM = {
    DIGITBIG_0,
    DIGITBIG_1,
    DIGITBIG_2,
    DIGITBIG_3,
    DIGITBIG_4,
    DIGITBIG_5,
    DIGITBIG_6,
    DIGITBIG_7,
    DIGITBIG_8,
    DIGITBIG_9
};

void drawDigit(int digit, int x, int y, uint16_t color);
void draw2DigitNumber(int number, int x, int y, uint16_t color);
void drawDigitBig(int digit, int x, int y, uint16_t color);
void draw2DigitBigNumber(int number, int x, int y, uint16_t color);
#endif