#ifndef PATTERN_TWINKLE_H
#define PATTERN_TWINKLE_H

#include "pattern.h"

class patternTwinkle : public Pattern
{
private:
    uint16_t _twinklingPixel;
public:
    patternTwinkle(Adafruit_NeoPixel * strip);
    void update(const uint16_t index, const uint32_t colour1, const uint32_t colour2);
    void start(const uint32_t colour1, const uint32_t colour2);
    uint16_t interval(const PatternSpeed s);
};

patternTwinkle::patternTwinkle(Adafruit_NeoPixel * strip) :
    Pattern(strip, 2, "Twinkle", 2),
    _twinklingPixel(strip->numPixels() + 1) {}

void patternTwinkle::start(const uint32_t colour1, const uint32_t colour2) {
    update(1, colour1, colour2);
}

void patternTwinkle::update(const uint16_t index, const uint32_t colour1, const uint32_t colour2) {
    const uint16_t pixelCount = _strip->numPixels();
    if (_twinklingPixel >= pixelCount) {
        if (!random(5)) {
            _twinklingPixel = random(pixelCount);
        }
    }
    else if (!random(2)) {
        _twinklingPixel = pixelCount + 1;
    }
    for(uint16_t i = 0; i < pixelCount; ++i) {
        _strip->setPixelColor(i, (i == _twinklingPixel) ? colour2 : colour1);
    }
    _strip->show();
}

uint16_t patternTwinkle::interval(const PatternSpeed s) {
    switch(s) {
    case PatternSpeed::VERY_SLOW:
        return 160;
    case PatternSpeed::SLOW:
        return 80;
    case PatternSpeed::MODERATE:
    default:
        return 40;
    case PatternSpeed::FAST:
        return 20;
    case PatternSpeed::LUDICROUS:
        return 10;
    }
}

#endif