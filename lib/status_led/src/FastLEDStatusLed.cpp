#include <Arduino.h>
#include "FastLEDStatusLed.h"

FastLEDStatusLed::FastLEDStatusLed(CLEDController &controller) : led(controller) {}

void FastLEDStatusLed::setColor(color_t color, float brightness)
{
    led.showColor(CRGB(color._red, color._green, color._blue), (int)brightness * 255);
}

void FastLEDStatusLed::clearColor()
{
    led.showColor(CRGB::Black);
}