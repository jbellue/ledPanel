#ifndef PATTERN_RAINBOW_CYCLE_H
#define PATTERN_RAINBOW_CYCLE_H

#include "pattern.h"
#include "utils.h"

class patternRainbowCycle : public Pattern
{
private:
    struct Segment {
        uint16_t first;
        uint16_t last;
    };
    static const uint8_t _num_segments = 3;
    Segment _segments[_num_segments];
    void initSegments();
    /* data */
public:
    patternRainbowCycle(Adafruit_NeoPixel * strip);
    void update(const uint16_t index);
    void start();
    uint16_t interval();
};

void patternRainbowCycle::initSegments() {
    const uint16_t pixelCount = _strip->numPixels();
    uint8_t ledsPerSegment[_num_segments];
    for (uint8_t i = 0; i < _num_segments; ++i) {
        ledsPerSegment[i] = pixelCount / _num_segments;
    }
    const uint8_t remainder = _strip->numPixels() % _num_segments;
    for (uint8_t i = 0; i < remainder; ++i) {
        ledsPerSegment[i] += 1;
    }
    _segments[0].first = 0;
    _segments[0].last = ledsPerSegment[0];
    _segments[1].first = ledsPerSegment[0] + 1;
    _segments[1].last = _segments[1].first + ledsPerSegment[1];
    _segments[2].first = ledsPerSegment[1] + 1;
    _segments[2].last = _segments[2].first + ledsPerSegment[2];
}
patternRainbowCycle::patternRainbowCycle(Adafruit_NeoPixel * strip) :
    Pattern(strip, 20, 255, 0x000000, 0x000000, "Rainbow Cycle") {
    initSegments();
}

void patternRainbowCycle::start() {
    update(0);
}
void patternRainbowCycle::update(const uint16_t index) {
    const uint16_t pixelCount = _strip->numPixels();
    for(uint16_t i = 0; i < pixelCount; ++i) {
        _strip->setPixelColor(i, colourWheel((i * 256 / pixelCount + index) & 255));
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