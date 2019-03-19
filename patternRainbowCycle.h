#ifndef PATTERN_RAINBOW_CYCLE_H
#define PATTERN_RAINBOW_CYCLE_H

#include "pattern.h"
#include "utils.h"

class patternRainbowCycle : public Pattern
{
private:
    static const uint8_t _segmentCount = 3;
    static const uint16_t _ledsPerSegment = 4;
public:
    patternRainbowCycle(Adafruit_NeoPixel * strip);
    void update(const uint16_t index);
    void start();
    uint16_t interval();
};

patternRainbowCycle::patternRainbowCycle(Adafruit_NeoPixel * strip) :
    Pattern(strip, 20, 255, 0x000000, 0x000000, "Rainbow Cycle") {
    }

void patternRainbowCycle::start() {
    update(0);
}
void patternRainbowCycle::update(const uint16_t index) {
    const uint16_t _segments[_segmentCount][_ledsPerSegment] = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};
    for(uint8_t i = 0; i < _segmentCount; ++i) {
        for(uint16_t j = 0; j < _ledsPerSegment; ++j) {
            _strip->setPixelColor(_segments[i][j], colourWheel((j * 256 / _ledsPerSegment + index) & 255));
        }
    }
    _strip->show();
}

uint16_t patternRainbowCycle::interval() {
    switch(_speed) {
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