#ifndef PATTERN_H
#define PATTERN_H

#include <Adafruit_NeoPixel.h>

enum PatternSpeed {
    VERY_SLOW,
    SLOW,
    MODERATE,
    FAST,
    LUDICROUS
};

class Pattern {
protected:
    Adafruit_NeoPixel* _strip;
    uint16_t _totalSteps;        // total number of steps in the pattern
    const char* _name;
    uint8_t _numColour;

public:
    Pattern(Adafruit_NeoPixel* strip, uint16_t totalSteps, const char* name, const uint8_t numColour) :
        _strip(strip),
        _totalSteps(totalSteps),
        _name(name),
        _numColour(numColour) {};
    virtual void update(const uint16_t index, const uint32_t colour1, const uint32_t colour2) = 0;
    virtual void start(const uint32_t colour1, const uint32_t colour2);
    virtual uint8_t numColour() {return _numColour;}

    virtual uint16_t interval(const PatternSpeed s);
    uint16_t totalSteps() {return _totalSteps;}
    const char* name() {return _name;}
};


#endif