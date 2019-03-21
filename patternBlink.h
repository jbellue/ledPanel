#ifndef PATTERN_BLINK_H
#define PATTERN_BLINK_H

#include "pattern.h"

class patternBlink : public Pattern
{
private:
    /* data */
public:
    patternBlink(Adafruit_NeoPixel * strip);
    void update(const uint16_t index, const uint32_t colour1, const uint32_t colour2);
    void start(const uint32_t colour1, const uint32_t colour2);
    uint16_t interval(const PatternSpeed s);
};

patternBlink::patternBlink(Adafruit_NeoPixel * strip) :
    Pattern(strip, 2, "Blink") {}

void patternBlink::start(const uint32_t colour1, const uint32_t colour2) {
    update(1, colour1, colour2);
}

void patternBlink::update(const uint16_t index, const uint32_t colour1, const uint32_t colour2) {
    const uint32_t c = (index ? colour1 : colour2);
    const uint16_t pixelCount = _strip->numPixels();
    for(uint16_t i = 0; i < pixelCount; ++i) {
        _strip->setPixelColor(i, c);
    }
    _strip->show();
}

uint16_t patternBlink::interval(const PatternSpeed s) {
    switch(s) {
    case PatternSpeed::VERY_SLOW:
        return 4000;
    case PatternSpeed::SLOW:
        return 2000;
    case PatternSpeed::FAST:
        return 500;
    case PatternSpeed::LUDICROUS:
        return 250;
    case PatternSpeed::MODERATE:
    default:
        return 1000;
    }
}

#endif