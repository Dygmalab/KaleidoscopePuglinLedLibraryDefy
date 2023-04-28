#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include <LEDManagement.hpp>
#endif

class LedModeSerializable_Breathe : public LedModeSerializable {
 public:
  explicit LedModeSerializable_Breathe(uint32_t id)
    : LedModeSerializable(id) {
  }

#ifdef KEYSCANNER
  void update() override {
    uint8_t i = ((uint16_t)to_ms_since_boot(get_absolute_time())) >> 4;

    if (i & 0x80) {
      i = 255 - i;
    }

    i           = i << 1;
    uint8_t ii  = (i * i) >> 8;
    uint8_t iii = (ii * i) >> 8;

    i = (((3 * (uint16_t)(ii)) - (2 * (uint16_t)(iii))) / 2) + 80;

    RGBW breathe = LEDManagement::HSVtoRGB(breatheHue, breatheSaturation, i);
    breathe.w    = 0;
    LEDManagement::set_all_leds(breathe);
  }
#else
  void update() override {
  }
#endif

  uint16_t breatheSaturation = 31;

 private:
  uint16_t breatheHue       = 4;
  uint8_t breatheLastUpdate = 0;
};

static LedModeSerializable_Breathe ledModeSerializableBreathe{CRC32_STR("LedModeSerializable_Breathe")};