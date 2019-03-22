#ifndef PATTERN_FADE_H
#define PATTERN_FADE_H

#include "pattern.h"
#include "utils.h"

/* This pattern has 32 steps so we can do a fast divide in its update, by byteshifting to the right. */
class patternFade : public Pattern
{
private:
    bool _goingToColour1;

    // this holds the number of bytes we need to shift
    // essentially will divide by 32, but more efficiently (until compilers get smart enough?)
    // if the number of steps changes, change that!
    static const uint8_t _log2steps = 5;
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
    const uint32_t colourOrigin = _goingToColour1 ? colour2 : colour1;
    const uint32_t colourDestination = _goingToColour1 ? colour1 : colour2;

    const uint8_t red = ((uint32toRed(colourOrigin) * (_totalSteps - index)) + (uint32toRed(colourDestination) * index)) >> _log2steps;
    const uint8_t green = ((uint32toGreen(colourOrigin) * (_totalSteps - index)) + (uint32toGreen(colourDestination) * index)) >> _log2steps;
    const uint8_t blue = ((uint32toBlue(colourOrigin) * (_totalSteps - index)) + (uint32toBlue(colourDestination) * index)) >> _log2steps;
    const uint32_t c = rgbToInt32(red, green, blue);
    const uint16_t pixelCount = _strip->numPixels();
    for(uint16_t i = 0; i < pixelCount; ++i) {
        _strip->setPixelColor(i, c);
    }
    _strip->show();

    // if that was the last step, change the direction
    if (index >= _totalSteps - 1) {
        _goingToColour1 = !_goingToColour1;
    }
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