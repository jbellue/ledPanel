#include "patternsManager.h"
#include "patternBlink.h"
#include "patternPlain.h"
#include "patternPlainRainbow.h"
#include "patternRainbowCycle.h"
#include "patternFade.h"

PatternsManager::PatternsManager(Adafruit_NeoPixel* strip) :
    _strip(strip),
    _pattern(PLAIN) {
}

void PatternsManager::begin() {
    _strip->begin();

    _patterns[PLAIN] = new patternPlain(_strip);
    _patterns[BLINK] = new patternBlink(_strip);
    _patterns[PLAIN_RAINBOW] = new patternPlainRainbow(_strip);
    _patterns[FADE] = new patternFade(_strip);
}

void PatternsManager::update() {
    if((millis() - _lastUpdate) > _patterns[_pattern]->interval()) // time to update
    {
        _lastUpdate = millis();
        _patterns[_pattern]->update(_index);
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
    _patterns[_pattern]->start();
}

void PatternsManager::speed(const uint8_t s) {
    if (s != PatternSpeed::VERY_SLOW &&
        s != PatternSpeed::SLOW &&
        s != PatternSpeed::FAST &&
        s != PatternSpeed::LUDICROUS) {
        _patterns[_pattern]->speed(PatternSpeed::MODERATE);
    }
    else {
        _patterns[_pattern]->speed((PatternSpeed)s);        
    }

}
// // Initialize for a Theater Chase
// void PatternsManager::theaterChase(const uint32_t colour1, const uint32_t colour2, const uint8_t interval) {
//     _interval = interval;
//     _totalSteps = _strip->numPixels();
//     _colour1 = colour1;
//     _colour2 = colour2;
//     _index = 0;
// }

// // Update the Theater Chase Pattern
// void PatternsManager::theaterChaseUpdate() {
//     const uint16_t pixelCount = _strip->numPixels();
//     for(uint16_t i=0; i < pixelCount; ++i) {
//         if ((i + _index) % 3 == 0) {
//             _strip->setPixelColor(i, _colour1);
//         }
//         else {
//             _strip->setPixelColor(i, _colour2);
//         }
//     }
//     _strip->show();
//     increment();
// }


// // Initialize for a SCANNNER
// void PatternsManager::scanner(const uint32_t colour1, const uint8_t interval) {
//     _interval = interval;
//     _totalSteps = (_strip->numPixels() - 1) * 2;
//     _colour1 = colour1;
//     _index = 0;
// }

// // Update the Scanner Pattern
// void PatternsManager::scannerUpdate() {
//     const uint16_t pixelCount = _strip->numPixels();
//     for (int i = 0; i < pixelCount; ++i) {
//         if (i == _index) {
//             _strip->setPixelColor(i, _colour1);
//         }
//         else if (i == _totalSteps - _index) {
//             _strip->setPixelColor(i, _colour1);
//         }
//         else {
//             _strip->setPixelColor(i, dimColor(_strip->getPixelColor(i)));
//         }
//     }
//     _strip->show();
//     increment();
// }

// // Initialize for a Fade
// void PatternsManager::fade(const uint32_t colour1, const uint32_t colour2, const uint16_t steps, const uint8_t interval) {
//     _interval = interval;
//     _totalSteps = steps;
//     _colour1 = colour1;
//     _colour2 = colour2;
//     _index = 0;
// }

// // Update the Fade Pattern
// void PatternsManager::fadeUpdate() {
//     // Calculate linear interpolation between _colour1 and _colour2
//     // Optimise order of operations to minimize truncation error
//     const uint8_t color_red = ((red(_colour1) * (_totalSteps - _index))
    // void colorWipeUpdate(); + (red(_colour2) * _index)) / _totalSteps;
//     const uint8_t color_green = ((green(_colour1) * (_totalSteps - _index)) + (green(_colour2) * _index)) / _totalSteps;
//     const uint8_t color_blue = ((blue(_colour1) * (_totalSteps - _index)) + (blue(_colour2) * _index)) / _totalSteps;
    
//     colorSet(Adafruit_NeoPixel::Color(color_red, color_green, color_blue));
//     _strip->show();
//     increment();
// }

// // Set all pixels to a color (synchronously)
// void PatternsManager::colorSet(const uint32_t color) {
//     const uint16_t pixelCount = _strip->numPixels();
//     for (int i = 0; i < pixelCount; ++i) {
//         _strip->setPixelColor(i, color);
//     }
//     _strip->show();
// }


