#ifndef PATTERN_RAINBOW_CYCLE_H
#define PATTERN_RAINBOW_CYCLE_H

#include "pattern.h"
#include "utils.h"

class patternRainbowCycle : public Pattern
{
private:
    uint16_t _ledsPerSegment;
public:
    patternRainbowCycle(Adafruit_NeoPixel * strip);
    patternRainbowCycle(Adafruit_NeoPixel * strip, const uint16_t ledPerSegment);
    void update(const uint16_t index);
    void start();
    uint16_t interval(const PatternSpeed s);
};

patternRainbowCycle::patternRainbowCycle(Adafruit_NeoPixel * strip) :
    patternRainbowCycle(strip, strip->numPixels()) {}

patternRainbowCycle::patternRainbowCycle(Adafruit_NeoPixel * strip, const uint16_t ledPerSegment) :
    Pattern(strip, 20, 255, 0x000000, 0x000000, "Rainbow Cycle"),
    _ledsPerSegment(ledPerSegment) {
}

void patternRainbowCycle::start() {
    update(0);
}
void patternRainbowCycle::update(const uint16_t index) {
    const uint16_t ledCount = _strip->numPixels();
    for(uint16_t i = 0; i < ledCount; ++i) {
        _strip->setPixelColor(i, colourWheel((((i % _ledsPerSegment) * 256 / ledCount) + index) & 255));
    }
    _strip->show();
}

uint16_t patternRainbowCycle::interval(const PatternSpeed s) {
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