#include "utils.h"

uint32_t rgbToInt32(const uint8_t r, const uint8_t g, const uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t colourWheel(uint8_t wheelPos) {
    wheelPos = 255 - wheelPos;
    if(wheelPos < 85) {
        return rgbToInt32(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    else if(wheelPos < 170) {
        wheelPos -= 85;
        return rgbToInt32(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    else {
        wheelPos -= 170;
        return rgbToInt32(wheelPos * 3, 255 - wheelPos * 3, 0);
    }
}

uint8_t uint32toRed(const uint32_t colour) {
    return (colour >> 16) & 0xFF;
}

uint8_t uint32toGreen(const uint32_t colour) {
    return (colour >> 8) & 0xFF;
}

uint8_t uint32toBlue(const uint32_t colour) {
    return colour & 0xFF;
}

uint32_t dimColour(const uint32_t colour) {
    // Shift R, G and B components one bit to the right
    return rgbToInt32(uint32toRed(colour) >> 1, uint32toGreen(colour) >> 1, uint32toBlue(colour) >> 1);
}