#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include <LEDManagement.hpp>
#endif

class LedModeSerializable_Rainbow : public LedModeSerializable {
 public:
  explicit LedModeSerializable_Rainbow(uint32_t id)
    : LedModeSerializable(id) {
  }

#ifdef KEYSCANNER
  void update() override {
    rainbowHue   = base_settings.step;
    RGBW rainbow = LEDManagement::HSVtoRGB(rainbowHue, rainbowSaturation, base_settings.brightness);
    rainbow.w    = 0;
    rainbowHue += 1;
    if (rainbowHue >= 255) {
      rainbowHue -= 255;
    }
    base_settings.step = rainbowHue;
    LEDManagement::set_all_leds(rainbow);
    LEDManagement::set_updated(true);
  }
#else
  void update() override {
    if (!kaleidoscope::Runtime.has_leds)
      return;

    if (!kaleidoscope::Runtime.hasTimeExpired(rainbowLastUpdate, base_settings.delay_ms)) {
      return;
    } else {
      rainbowLastUpdate += base_settings.delay_ms;
    }
    //Only for the neuron
    rainbowHue       = base_settings.step;
    uint16_t led_hue = rainbowHue + 16 * (kaleidoscope::Runtime.device().LEDs().all().end().offset() / 4);
    // We want led_hue to be capped at 255, but we do not want to clip it to
    // that, because that does not result in a nice animation. Instead, when it
    // is higher than 255, we simply substract 255, and repeat that until we're
    // within cap. This lays out the rainbow in a kind of wave.
    while (led_hue > 255) {
      led_hue -= 255;
    }
    cRGB rainbow = hsvToRgb(led_hue, rainbowSaturation, base_settings.brightness);
    rainbow.w    = 0;
    rainbowHue += 1;
    if (rainbowHue >= 255) {
      rainbowHue -= 255;
    }
    base_settings.step = rainbowHue;
    kaleidoscope::Runtime.device().ledDriver().setCrgbNeuron(rainbow);
  }
#endif

 private:
  uint16_t rainbowHue        = 0;
  uint16_t rainbowSaturation = 255;
  uint8_t rainbowLastUpdate  = 0;
};

static LedModeSerializable_Rainbow ledModeSerializableRainbow{CRC32_STR("LedModeSerializable_Rainbow")};