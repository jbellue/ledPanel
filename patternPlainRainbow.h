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
    void update(const uint16_t index);
    void start();
    uint16_t interval(const PatternSpeed s);
};

patternPlainRainbow::patternPlainRainbow(Adafruit_NeoPixel * strip) :
    Pattern(strip, 20, 255, 0x000000, 0x000000, "Plain Rainbow") {}

void patternPlainRainbow::start() {
    update(0);
}

void patternPlainRainbow::update(const uint16_t index) {
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