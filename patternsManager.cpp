#include "patternsManager.h"
#include "patternBlink.h"
#include "patternPlain.h"
#include "patternPlainRainbow.h"
#include "patternRainbowCycle.h"
#include "patternTinwkle.h"
#include "patternStatic.h"
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
    _patterns[TWINKLE] = new patternTwinkle(_strip);
    _patterns[STATIC] = new patternStatic(_strip);
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

void PatternsManager::getPatternsInfo(const char** dest, uint8_t numColour[]) {
    for(uint8_t i = 0; i <= LAST_PATTERN; ++i) {
        dest[i] = _patterns[i]->name();
        numColour[i] = _patterns[i]->numColour();
    }
}

void PatternsManager::increment() {
    if (++_index >= _patterns[_pattern]->totalSteps()) {
        _index = 0;
    }
}

bool PatternsManager::changePattern(const uint8_t newPattern) {
    if (newPattern > LAST_PATTERN) {
        return false;
    }
    _pattern = (PatternChoice)newPattern;
    _index = 0;
    _patterns[_pattern]->start(_colour1, _colour2);
}

void PatternsManager::speed(const uint8_t s) {
    // s being unsigned, it can't be less than PatternSpeed::VERY_SLOW, 
    // so it makes sense to only check if it's more than the fastest PatternSpeed
    if (s > PatternSpeed::LUDICROUS) {
        _speed = PatternSpeed::MODERATE;
    }
    else {
        _speed = (PatternSpeed)s;        
    }
}

void PatternsManager::brightness(const uint8_t brightness) {
    _strip->setBrightness(brightness);
    _strip->show();
}

