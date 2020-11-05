#ifndef PATTERN_STATIC_H
#define PATTERN_STATIC_H

#include "pattern.h"

class patternStatic : public Pattern
{
private:
    void shuffleArray(uint16_t * array, const uint16_t size);
    uint16_t _ledIndex[256];
public:
    patternStatic(Adafruit_NeoPixel * strip);
    void update(const uint16_t index, const uint32_t colour1, const uint32_t colour2);
    void start(const uint32_t colour1, const uint32_t colour2);
    uint16_t interval(const PatternSpeed s);
};

patternStatic::patternStatic(Adafruit_NeoPixel * strip) :
    Pattern(strip, 2, "Static", 2) {
        const uint16_t pixelCount = _strip->numPixels();
        for(uint16_t i = 0; i < pixelCount; ++i) {
            _ledIndex[i] = i;
        }
    }

void patternStatic::start(const uint32_t colour1, const uint32_t colour2) {
    update(1, colour1, colour2);
}

void patternStatic::update(const uint16_t index, const uint32_t colour1, const uint32_t colour2) {
    const uint16_t pixelCount = _strip->numPixels();
    shuffleArray(_ledIndex, pixelCount);
    const uint16_t halfPixels = pixelCount >> 2;
    for(uint16_t i = 0; i < pixelCount; ++i) {
        _strip->setPixelColor(_ledIndex[i], (i <= halfPixels ? colour1 : colour2));
    }
    _strip->show();
}

uint16_t patternStatic::interval(const PatternSpeed s) {
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

void patternStatic::shuffleArray(uint16_t* array, const uint16_t size) {
    uint16_t last = 0;
    uint16_t temp = array[last];
    for (uint16_t i = 0; i < size; ++i) {
        const uint16_t index = random(size);
        array[last] = array[index];
        last = index;
    }
    array[last] = temp;
}
#endif