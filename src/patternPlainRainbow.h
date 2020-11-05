#ifndef PATTERN_PLAIN_RAINBOW_H
#define PATTERN_PLAIN_RAINBOW_H

#include "pattern.h"
#include "utils.h"

class patternPlainRainbow : public Pattern
{
private:
    /* data */
public:
    patternPlainRainbow(Adafruit_NeoPixel * strip);
    void update(const uint16_t index, const uint32_t colour1, const uint32_t colour2);
    void start(const uint32_t colour1, const uint32_t colour2);
    uint16_t interval(const PatternSpeed s);
};

patternPlainRainbow::patternPlainRainbow(Adafruit_NeoPixel * strip) :
    Pattern(strip, 255, "Plain Rainbow", 0) {}

void patternPlainRainbow::start(const uint32_t colour1, const uint32_t colour2) {
    update(0, colour1, colour2);
}

void patternPlainRainbow::update(const uint16_t index, const uint32_t colour1, const uint32_t colour2) {
    const uint16_t pixelCount = _strip->numPixels();
    for(uint16_t i = 0; i < pixelCount; ++i) {
        _strip->setPixelColor(i, colourWheel(index));
    }
    _strip->show();
}


uint16_t patternPlainRainbow::interval(const PatternSpeed s) {
    switch(s) {
    case PatternSpeed::VERY_SLOW:
        return 80;
    case PatternSpeed::SLOW:
        return 40;
    case PatternSpeed::FAST:
        return 10;
    case PatternSpeed::LUDICROUS:
        return 5;
    case PatternSpeed::MODERATE:
    default:
        return 20;
    }
}

#endif