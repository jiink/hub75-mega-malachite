#include <stdint.h>
#include <pgmspace.h>
#ifndef CITY_H
#define CITY_H

void citySetup(void);
void cityLoop(void);

#define PXBG_0_HEIGHT 32
#define PXBG_0_WIDTH 64

// array size is 256
static const uint8_t pxbg_0[] PROGMEM  = {
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00011111, 0B10000000, 0B00000000, 0B00001111, 0B11100000, 0B00000010, 0B00000100, 0B00000000, 
  0B00011111, 0B10000000, 0B00000000, 0B00001111, 0B11100000, 0B00000111, 0B11111110, 0B00000000, 
  0B00011111, 0B10000000, 0B00000000, 0B00001111, 0B11100000, 0B00000111, 0B11111110, 0B00000000, 
  0B00011111, 0B10000000, 0B00000000, 0B00001111, 0B11100000, 0B00000111, 0B11111110, 0B00000000, 
  0B00011111, 0B10000001, 0B11111100, 0B00001111, 0B11100000, 0B00000111, 0B11111110, 0B00000000, 
  0B00011111, 0B10000001, 0B11111100, 0B00001111, 0B11100000, 0B00000111, 0B11111110, 0B01111111, 
  0B00011111, 0B10000001, 0B11111100, 0B00001111, 0B11100111, 0B10001111, 0B11111110, 0B01111111, 
  0B00011111, 0B10000001, 0B11111110, 0B00001111, 0B11100111, 0B10001111, 0B11111110, 0B01111111, 
  0B00011111, 0B11111001, 0B11111100, 0B00001111, 0B11101111, 0B10000111, 0B11111110, 0B01111111, 
  0B00011111, 0B11111001, 0B11111100, 0B00001111, 0B11101111, 0B10000111, 0B11111110, 0B01111111, 
  0B00011111, 0B11111001, 0B11111100, 0B00001111, 0B11111111, 0B10000111, 0B11111110, 0B01111111
};


#define PXBG_1_HEIGHT 32
#define PXBG_1_WIDTH 64

// array size is 256
static const uint8_t pxbg_1[] PROGMEM  = {
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00111000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00111000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00111000, 0B00110000, 0B00000000, 
  0B01000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00111000, 0B00111000, 0B00000000, 
  0B11110000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00111000, 0B00111100, 0B00111100, 
  0B11110000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00111000, 0B00111110, 0B00111100, 
  0B11110000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00111000, 0B00111110, 0B00111100, 
  0B11110000, 0B00000000, 0B00000001, 0B11000000, 0B00000000, 0B00111000, 0B00111110, 0B00111100, 
  0B11110000, 0B00000000, 0B00000001, 0B11000001, 0B11100000, 0B00111000, 0B00111110, 0B00111100, 
  0B11110000, 0B00011110, 0B00000001, 0B11000001, 0B11100011, 0B10111000, 0B00111110, 0B00111100, 
  0B11110000, 0B00011110, 0B00000001, 0B11000001, 0B11100011, 0B11111111, 0B11111111, 0B11111111, 
  0B11110001, 0B11111111, 0B11110001, 0B11111111, 0B11111111, 0B10111111, 0B11111111, 0B11111100, 
  0B11110001, 0B11111111, 0B11110001, 0B11111111, 0B11111111, 0B10111111, 0B11111111, 0B11111100, 
  0B11110001, 0B11111111, 0B11110001, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111100, 
  0B11110001, 0B11111111, 0B11110001, 0B11111111, 0B11111111, 0B10111111, 0B11111111, 0B11111100, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111
};


#define PXBG_2_HEIGHT 32
#define PXBG_2_WIDTH 64

// array size is 256
static const uint8_t pxbg_2[] PROGMEM  = {
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B00000000, 0B00000100, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 
  0B01000000, 0B00000100, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B00000000, 0B10000000, 
  0B01000000, 0B00000100, 0B01100000, 0B00110000, 0B00000000, 0B00000000, 0B00000000, 0B10000000, 
  0B01000000, 0B11000100, 0B01100000, 0B00110001, 0B00000000, 0B00000000, 0B00000000, 0B10000000, 
  0B01000010, 0B11110101, 0B01101100, 0B00110001, 0B00000000, 0B00000000, 0B10010000, 0B10000000, 
  0B01001111, 0B11111111, 0B11101101, 0B00110001, 0B00000110, 0B00000010, 0B10010000, 0B10000000, 
  0B11111111, 0B11111111, 0B11111111, 0B11111101, 0B00010110, 0B10010010, 0B11110100, 0B10111001, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 
  0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111, 0B11111111
};

#endif