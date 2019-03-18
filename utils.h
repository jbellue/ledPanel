#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

uint32_t rgbToInt32(const uint8_t r, const uint8_t g, const uint8_t b);

// Input a value 0 to 255 to get a colour value.
// The colours are a transition r - g - b - back to r.
uint32_t colourWheel(uint8_t wheelPos);

// return the Red, Green and Blue parts of a uint32_t colour
uint8_t uint32toRed(const uint32_t colour);
uint8_t uint32toGreen(const uint32_t colour);
uint8_t uint32toBlue(const uint32_t colour);

// Calculate 50% dimmed version of a colour
uint32_t dimColour(const uint32_t colour);

#endif