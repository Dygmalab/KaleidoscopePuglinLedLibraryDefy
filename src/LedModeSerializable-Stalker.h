/* LedModeSerializable_Stalker - LED Stalker effect that is then serialized to the keyscanner when executed.
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

#ifdef KEYSCANNER
#include <LEDManagement.hpp>
#include <Keyscanner.hpp>
#include <array>
#include <functional>
#endif

class LedModeSerializable_Stalker : public LedModeSerializable {
 public:
  explicit LedModeSerializable_Stalker(uint32_t id)
    : LedModeSerializable(id) {
  }

  uint8_t serialize(uint8_t *output) const override {
    uint8_t index   = LedModeSerializable::serialize(output);
    output[index]   = compute_;
    output[++index] = r_;
    output[++index] = g_;
    output[++index] = b_;
    output[++index] = w_;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index = LedModeSerializable::deSerialize(input);
    compute_      = (ComputeModes)input[index];
    r_            = input[++index + 1];
    g_            = input[++index + 2];
    b_            = input[++index + 3];
    w_            = input[++index + 4];
#ifdef KEYSCANNER
    switch (compute_) {
    case RAINBOW:
      compute_function_ = rainbow_compute;
      break;
    case HAUNT:
      compute_function_ = [this](uint8_t &step) {
        return this->haunt_compute(step);
      };
      break;
    case BLAZING_TRAIL:
      compute_function_ = blazingTrail_compute;
      break;
    }
    map = {};
#endif
    base_settings.delay_ms = 50;
    return index;
  }
#ifdef KEYSCANNER
  static RGBW rainbow_compute(uint8_t &step) {
    if (step > 0)
      step -= 1;
    else
      step = 0;

    const RGBW &rgb = LEDManagement::HSVtoRGB(255 - step, 255, step);
    return rgb;
  }


  RGBW haunt_compute(uint8_t &step) {
    RGBW color = {(uint8_t)std::min(step * r_ / 255, 255),
                  (uint8_t)std::min(step * g_ / 255, 255),
                  (uint8_t)std::min(step * b_ / 255, 255),
                  (uint8_t)std::min(step * w_ / 255, 255)};

    if (step >= 0xf0)
      step -= 1;
    else if (step >= 0x40)
      step -= 16;
    else if (step >= 32)
      step -= 32;
    else
      step = 0;

    return color;
  }

  static RGBW blazingTrail_compute(uint8_t &step) {
    static constexpr uint8_t hue_start = 50.0 / 360 * 0xff;
    static constexpr uint8_t hue_end   = 0;

    cRGB color;

    // Get the position in the animation, where 0 is start and 0xff is end
    uint8_t pos255 = 0xff - step;

    // Fade hue linearly from orange to red
    uint8_t hue = hue_start + (pos255 * (hue_end - hue_start) >> 8);

    // Fade value from full following a 1-x^4 curve
    uint8_t val =
      0xff                                            // Maximum brightness
      - ((uint32_t)pos255 * pos255 * pos255 * pos255  // Animation position to 4th power
         >> 24)                                       // ...pulled down to 8-bit range (but this has a maximum of 0xfc rather than 0xff)
      - pos255 / (0x100 / 4);                         // Correction to bring the end result into a full 0 to 0xff range

    color = LEDManagement::HSVtoRGB(hue, 0xff, val);

    if (step < 4) {
      step = 0;
    } else {
      step -= 4;
    }

    return color;
  }

  inline static std::array<std::array<uint8_t, KeyScanner::COLS>, KeyScanner::ROWS> map{};

  void update() override {
    uint8_t data[KeyScanner::ROWS]{};
    KeyScanner.get_key_matrix(data);
    for (int i = 0; i < KeyScanner::ROWS; ++i) {
      uint8_t rows = data[i];
      for (int j = 0; j < KeyScanner::COLS; ++j) {
        if (rows >> j & 1)
          map[i][j] = 0xFF;

        uint8_t &step = map[i][j];
        if (!step) continue;
        uint8_t pos = i * (KeyScanner::ROWS + 2) + j;
        if (pos > 26) {
          pos--;
        }
        if (step) {
          RGBW color = compute_function_(step);
          LEDManagement::set_led_at(color, pos);
        }
        if (!step) {
          LEDManagement::set_led_at({}, pos);
        }
      }
    }
  }
#else
  void update() override {
  }
#endif
  uint8_t r_;
  uint8_t g_;
  uint8_t b_;
  uint8_t w_;
  enum ComputeModes : uint8_t {
    RAINBOW,
    HAUNT,
    BLAZING_TRAIL
  };
  ComputeModes compute_;

 private:
#ifdef KEYSCANNER
  std::function<RGBW(uint8_t &)> compute_function_;
#endif
};

static LedModeSerializable_Stalker ledModeSerializableStalker{CRC32_STR("LedModeSerializable_Stalker")};