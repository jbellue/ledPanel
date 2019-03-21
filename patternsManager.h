#ifndef PATTERNSMANAGER_H
#define PATTERNSMANAGER_H

#include <Adafruit_NeoPixel.h>
#include "pattern.h"

enum PatternChoice : uint8_t {
    PLAIN = 0,
    BLINK,
    PLAIN_RAINBOW,
    RAINBOW_CYCLE,
    FADE,
    LAST_PATTERN = FADE
};

class PatternsManager {
public:
    PatternsManager(Adafruit_NeoPixel* strip);

    void getPatternsName(const char** dest);
    
    void begin();

    bool changePattern(uint8_t newPattern);

    // Update the pattern
    void update();

    uint32_t colour1() {return _patterns[_pattern]->colour1();}
    void colour1(const uint32_t c) {_patterns[_pattern]->colour1(c);}
    uint32_t colour2() {return _patterns[_pattern]->colour2();}
    void colour2(const uint32_t c) {_patterns[_pattern]->colour2(c);}
    PatternSpeed speed() {return _patterns[_pattern]->speed();}
    void speed(const uint8_t newSpeed);
private:
    PatternChoice _pattern;
    Adafruit_NeoPixel* _strip;
    Pattern* _patterns[LAST_PATTERN];

    unsigned long _interval;   // milliseconds between updates
    unsigned long _lastUpdate; // last update of position

    uint32_t _colour1, _colour2;  // What colors are in use
    uint16_t _totalSteps;  // total number of steps in the pattern
    uint16_t _index;  // current step within the pattern

    // Increment the index and reset at the end
    void increment();
};

#endif