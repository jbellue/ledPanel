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
    uint16_t _interval;
    uint16_t _totalSteps;        // total number of steps in the pattern
    uint32_t _colour1, _colour2; // What colors are in use
    const char* _name;
    PatternSpeed _speed;

public:
    Pattern(Adafruit_NeoPixel* strip, uint16_t interval, uint16_t totalSteps, uint32_t colour1, uint32_t colour2, const char* name) :
        _strip(strip),
        _interval(interval),
        _totalSteps(totalSteps),
        _colour1(colour1),
        _colour2(colour2),
        _name(name),
        _speed(PatternSpeed::MODERATE) {};
    virtual void update(const uint16_t index) = 0;
    virtual void start();

    PatternSpeed speed() {return _speed;}
    void speed(PatternSpeed s) {_speed = s;}
    virtual uint16_t interval();
    uint16_t totalSteps() {return _totalSteps;}
    uint32_t colour1() {return _colour1;}
    uint32_t colour2() {return _colour2;}
    virtual void colour1(uint32_t colour) {_colour1 = colour;}
    void colour2(uint32_t colour) {_colour2 = colour;}
    const char* name() {return _name;}
};


#endif