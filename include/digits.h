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

#endif