#ifndef PATTERN_PLAIN_H
#define PATTERN_PLAIN_H

#include "pattern.h"

class patternPlain : public Pattern
{
private:
    uint32_t _oldColour1;
public:
    patternPlain(Adafruit_NeoPixel * strip);
    void update(const uint16_t index, const uint32_t colour1, const uint32_t colour2);
    void start(const uint32_t colour1, const uint32_t colour2);
    uint16_t interval(const PatternSpeed s);
};

patternPlain::patternPlain(Adafruit_NeoPixel * strip) :
    Pattern(strip, 1, "Plain"),
    _oldColour1(0) {}

void patternPlain::start(const uint32_t colour1, const uint32_t colour2) {
    update(0, colour1, colour2);
}

void patternPlain::update(const uint16_t index, const uint32_t colour1, const uint32_t colour2) {
    if (colour1 != _oldColour1) {
        const uint16_t pixelCount = _strip->numPixels();
        for(uint16_t i = 0; i < pixelCount; ++i) {
            _strip->setPixelColor(i, colour1);
        }
        _oldColour1 = colour1;
        _strip->show();
    }
}

uint16_t patternPlain::interval(const PatternSpeed s) {
    return 50;
}

#endif