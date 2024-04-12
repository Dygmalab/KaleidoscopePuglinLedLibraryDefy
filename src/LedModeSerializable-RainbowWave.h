/* LedModeSerializable_RainbowWave - LED Rainbow wave effect that is then serialized to the keyscanner when executed.
 * Copyright (C) 2023, 2024  DygmaLabs, S. L.
 *
 * The MIT License (MIT)
 * Copyright © 2024 <copyright holders>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the “Software”), to deal in the
 * Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

class LedModeSerializable_RainbowWave : public LedModeSerializable {
 public:
  explicit LedModeSerializable_RainbowWave(uint32_t id)
    : LedModeSerializable(id) {
  }

#ifdef KEYSCANNER
  void update() override {
    //TODO Refactor this
    uint8_t multiplier = 0;
    if (gpio_get(25)) {
      multiplier = NUMBER_OF_LEDS;
    } else {
      multiplier = 0;
    }

    rainbowHue = base_settings.step;
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++) {
      uint16_t led_hue = rainbowHue + 16 * ((i + multiplier) / 4);
      // We want led_hue to be capped at 255, but we do not want to clip it to
      // that, because that does not result in a nice animation. Instead, when it
      // is higher than 255, we simply substract 255, and repeat that until we're
      // within cap. This lays out the rainbow in a kind of wave.
      while (led_hue > 255) {
        led_hue -= 255;
      }

      RGBW rainbow = LEDManagement::HSVtoRGB(led_hue, rainbowSaturation, base_settings.brightness);
      rainbow.w    = 0;
      LEDManagement::set_led_at(rainbow, i);
    }
    rainbowHue += 1;
    if (rainbowHue >= 255) {
      rainbowHue -= 255;
    }
    base_settings.step = rainbowHue;
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

static LedModeSerializable_RainbowWave ledModeSerializableRainbowWave{CRC32_STR("LedModeSerializable_RainbowWave")};