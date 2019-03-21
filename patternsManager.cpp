#include "patternsManager.h"
#include "patternBlink.h"
#include "patternPlain.h"
#include "patternPlainRainbow.h"
#include "patternRainbowCycle.h"
#include "patternFade.h"

PatternsManager::PatternsManager(Adafruit_NeoPixel* strip) :
    _strip(strip),
    _pattern(PLAIN),
    _colour1(0xFF0000),
    _colour2(0x00FF00) {
}

void PatternsManager::begin() {
    _strip->begin();

    const uint8_t numberOfSegments = 4;
    _patterns[PLAIN] = new patternPlain(_strip);
    _patterns[BLINK] = new patternBlink(_strip);
    _patterns[PLAIN_RAINBOW] = new patternPlainRainbow(_strip);
    _patterns[RAINBOW_CYCLE] = new patternRainbowCycle(_strip, numberOfSegments);
    _patterns[FADE] = new patternFade(_strip);

    _patterns[_pattern]->start(_colour1, _colour2);
}

void PatternsManager::update() {
    if((millis() - _lastUpdate) > _patterns[_pattern]->interval(_speed)) {
        _lastUpdate = millis();
        _patterns[_pattern]->update(_index, _colour1, _colour2);
        increment();
    }
}

void PatternsManager::getPatternsName(const char** dest) {
    for(uint8_t i = 0; i <= LAST_PATTERN; ++i) {
        dest[i] = _patterns[i]->name();
    }
}

void PatternsManager::increment() {
    if (++_index >= _patterns[_pattern]->totalSteps()) {
        _index = 0;
    }
}

bool PatternsManager::changePattern(const uint8_t newPattern) {
    if (newPattern < PLAIN || newPattern > LAST_PATTERN) {
        return false;
    }
    _pattern = (PatternChoice)newPattern;
    _index = 0;
    _patterns[_pattern]->start(_colour1, _colour2);
}

void PatternsManager::speed(const uint8_t s) {
    if (s != PatternSpeed::VERY_SLOW &&
        s != PatternSpeed::SLOW &&
        s != PatternSpeed::FAST &&
        s != PatternSpeed::LUDICROUS) {
        _speed = PatternSpeed::MODERATE;
    }
    else {
        _speed = (PatternSpeed)s;        
    }
}

