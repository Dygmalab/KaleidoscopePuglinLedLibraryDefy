#pragma once
// Juan
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

  inline static std::array<std::array<uint8_t, Pins::COLS>, Pins::ROWS> map{};

  void update() override {
    uint8_t data[Pins::ROWS]{};
    KeyScanner.readMatrix(data);
    for (int i = 0; i < Pins::ROWS; ++i) {
      uint8_t rows = data[i];
      for (int j = 0; j < Pins::COLS; ++j) {
        if (rows >> j & 1 )
          map[i][j] = 0xFF;

        uint8_t &step = map[i][j];
        if (!step) continue;


        #ifdef RAISE2
        uint8_t led_position;
          //uint8_t pos = i * Pins::COLS + j;
        if (gpio_get(Pins::SIDE_ID)) {
          //Right side.
           led_position = pos_right[i][j];
        } else {
          //Left side.
           led_position = pos_left[i][j];
        }

        #else
        uint8_t pos = i * (Pins::ROWS + 2) + j;
        if (pos > 26) {
          pos--;
        }
        #endif

        if (step) {
          RGBW color = compute_function_(step);
          LEDManagement::set_led_at(color, led_position);
        }
        if (!step) {
          LEDManagement::set_led_at({}, led_position);
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

  uint8_t pos_left[Pins::ROWS][Pins::COLS] = {
    {0,1, 2, 3,4,5,6},
    {7,8, 9, 10,11,12},
    {13,14, 15, 16,17,18},
    {19,20, 21, 22,23,24,25},
    {26,27, 28, 29,30,31,32,33},
  };

  uint8_t pos_right[Pins::ROWS][Pins::COLS] = {
    {0,1, 2, 3,4,5,6},
    {15,8, 9, 10,11,12,13,14},
    {7,16, 17, 18,19,20,21},
    {22,23, 24, 25,26,27},
    {28,29, 30, 31,32,33,34,35},
  };

#endif
};

static LedModeSerializable_Stalker ledModeSerializableStalker{CRC32_STR("LedModeSerializable_Stalker")};