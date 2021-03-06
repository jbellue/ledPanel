#ifndef PATTERNSMANAGER_H
#define PATTERNSMANAGER_H

#include <Adafruit_NeoPixel.h>
#include "pattern.h"

enum PatternChoice : uint8_t {
    PLAIN = 0,
    BLINK,
    PLAIN_RAINBOW,
    RAINBOW_CYCLE,
    TWINKLE,
    STATIC,
    FADE,
    LAST_PATTERN = FADE
};

class PatternsManager {
public:
    PatternsManager(Adafruit_NeoPixel* strip);

    void getPatternsInfo(const char** dest, uint8_t numColour[]);

    void begin();

    bool changePattern(uint8_t newPattern);

    // Update the pattern
    void update();

    uint32_t colour1() {return _colour1;}
    void colour1(const uint32_t c) {_colour1 = c;}
    uint32_t colour2() {return _colour2;}
    void colour2(const uint32_t c) {_colour2 = c;}
    uint8_t speed() {return _speed;}
    void speed(const uint8_t newSpeed);
    uint8_t pattern() {return _pattern;}
    uint8_t brightness() {return _strip->getBrightness();}
    void brightness(const uint8_t brightness);
private:
    Adafruit_NeoPixel* _strip;
    PatternChoice _pattern;
    Pattern* _patterns[LAST_PATTERN + 1];
    PatternSpeed _speed;

    unsigned long _lastUpdate; // last update of position

    uint32_t _colour1, _colour2;  // What colors are in use
    uint16_t _index;  // current step within the pattern

    // Increment the index and reset at the end
    void increment();
};

#endif