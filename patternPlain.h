#ifndef PATTERN_PLAIN_H
#define PATTERN_PLAIN_H

#include "pattern.h"

class patternPlain : public Pattern
{
private:
    /* data */
public:
    patternPlain(Adafruit_NeoPixel * strip);
    void update(const uint16_t index);
    void start();
    void colour1(uint32_t colour);
    uint16_t interval();
};

patternPlain::patternPlain(Adafruit_NeoPixel * strip) :
    Pattern(strip, 0, 1, 0xFF0000, 0x00FF00, "Plain") {}

void patternPlain::start() {
    colour1(0xFF0000);
}
void patternPlain::update(const uint16_t index) { }

void patternPlain::colour1(uint32_t colour) {
    const uint16_t pixelCount = _strip->numPixels();
    for(uint16_t i = 0; i < pixelCount; ++i) {
        _strip->setPixelColor(i, colour);
    }
    _strip->show();
}


uint16_t patternPlain::interval() {
    return UINT16_MAX;
}

#endif