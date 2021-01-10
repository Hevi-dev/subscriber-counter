#pragma once

#include <APA102.h>

template<uint8_t dataPin, uint8_t clockPin> class StatusLed {
  public:
    void setColor(rgb_color color, float brightness = 1.0) {
      led.write(&color, 1, uint8_t(brightness * 31));
    }

    void clearColor() {
      this->setColor(rgb_color(0,0,0), 0.0);
    }

  private:
    APA102<dataPin, clockPin> led;
};