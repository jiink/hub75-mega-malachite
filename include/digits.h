#include <stdint.h>
#include <pgmspace.h>
#ifndef DIGITS_H
#define DIGITS_H

#define DIGIT_WIDTH  3
#define DIGIT_HEIGHT 5

static const uint8_t digit0[] PROGMEM  = {
    0b111,
    0b101,
    0b101,
    0b101,
    0b111
};
static const uint8_t digit1[] PROGMEM  = {
    0b010,
    0b110,
    0b010,
    0b010,
    0b111
};
static const uint8_t digit2[] PROGMEM  = {
    0b111,
    0b001,
    0b111,
    0b100,
    0b111
};
static const uint8_t digit3[] PROGMEM  = {
    0b111,
    0b001,
    0b111,
    0b001,
    0b111
};
static const uint8_t digit4[] PROGMEM  = {
    0b101,
    0b101,
    0b111,
    0b001,
    0b001
};
static const uint8_t digit5[] PROGMEM  = {
    0b111,
    0b100,
    0b111,
    0b001,
    0b111
};
static const uint8_t digit6[] PROGMEM  = {
    0b111,
    0b100,
    0b111,
    0b101,
    0b111
};
static const uint8_t digit7[] PROGMEM  = {
    0b111,
    0b001,
    0b001,
    0b001,
    0b001
};
static const uint8_t digit8[] PROGMEM  = {
    0b111,
    0b101,
    0b111,
    0b101,
    0b111
};
static const uint8_t digit9[] PROGMEM  = {
    0b111,
    0b101,
    0b111,
    0b001,
    0b111
};
const uint8_t* const digits[] PROGMEM = {
    digit0,
    digit1,
    digit2,
    digit3,
    digit4,
    digit5,
    digit6,
    digit7,
    digit8,
    digit9
};

#endif