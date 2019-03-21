#ifndef PATTERN_FADE_H
#define PATTERN_FADE_H

#include "pattern.h"
#include "utils.h"

class patternFade : public Pattern
{
private:
    bool _goingToColour1;
public:
    patternFade(Adafruit_NeoPixel * strip);
    void update(const uint16_t index, const uint32_t colour1, const uint32_t colour2);
    void start(const uint32_t colour1, const uint32_t colour2);
    uint16_t interval(const PatternSpeed s);
};

patternFade::patternFade(Adafruit_NeoPixel * strip) :
    Pattern(strip, 32, "Fade"),
    _goingToColour1(false) {}

void patternFade::start(const uint32_t colour1, const uint32_t colour2) {
    update(0, colour1, colour2);
}

void patternFade::update(const uint16_t index, const uint32_t colour1, const uint32_t colour2) {
    if (index >= _totalSteps - 1) {
        _goingToColour1 = !_goingToColour1;
    }
    const uint32_t colourOrigin = _goingToColour1 ? colour2 : colour1;
    const uint32_t colourDestination = _goingToColour1 ? colour1 : colour2;

    uint8_t red =   ((uint32toRed(colourOrigin) * (_totalSteps - index)) + (uint32toRed(colourDestination) * index)) / _totalSteps;
    uint8_t green = ((uint32toGreen(colourOrigin) * (_totalSteps - index)) + (uint32toGreen(colourDestination) * index)) / _totalSteps;
    uint8_t blue =  ((uint32toBlue(colourOrigin) * (_totalSteps - index)) + (uint32toBlue(colourDestination) * index)) / _totalSteps;
    const uint32_t c = rgbToInt32(red, green, blue);
    const uint16_t pixelCount = _strip->numPixels();
    for(uint16_t i = 0; i < pixelCount; ++i) {
        _strip->setPixelColor(i, c);
    }
    _strip->show();
}

uint16_t patternFade::interval(const PatternSpeed s) {
    switch(s) {
    case PatternSpeed::VERY_SLOW:
        return 400;
    case PatternSpeed::SLOW:
        return 200;
    case PatternSpeed::FAST:
        return 50;
    case PatternSpeed::LUDICROUS:
        return 25;
    case PatternSpeed::MODERATE:
    default:
        return 100;
    }
}

#endif