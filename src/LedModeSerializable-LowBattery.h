#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#include "RFGW_communications.h"
#endif

class LedModeSerializable_LowBattery : public LedModeSerializable {
 public:
  explicit LedModeSerializable_LowBattery(uint32_t id)
    : LedModeSerializable(id) {
  }
  uint8_t serialize(uint8_t *output) const override {
    uint8_t index = LedModeSerializable::serialize(output);
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index = LedModeSerializable::deSerialize(input);
    return ++index;
  }
#ifdef NEURON_WIRED
  void update() override {
  }
#endif

#ifdef KEYSCANNER

  void update() override {
    RGBW first_cell, second_cell, third_cell = {0, 0, 0, 0};
    /*Column effect*/
    static enum {
      FIRST_CELL,
      SECOND_CELL,
      THIRD_CELL,
      NO_CELL,
    } currentCell = NO_CELL;
    switch (currentCell) {
    case NO_CELL:
      first_cell  = red;
      second_cell = red;
      third_cell  = red;
      currentCell = THIRD_CELL;
      break;
    case THIRD_CELL:
      first_cell  = ledOff;
      second_cell = red;
      third_cell  = red;
      currentCell = SECOND_CELL;
      break;
    case SECOND_CELL:
      first_cell  = ledOff;
      second_cell = ledOff;
      third_cell  = red;
      currentCell = FIRST_CELL;
      break;
    case FIRST_CELL:
      first_cell  = ledOff;
      second_cell = ledOff;
      third_cell  = ledOff;
      currentCell = NO_CELL;
      break;
    }
    counter--;
    LEDManagement::set_led_at(first_cell, 6);
    LEDManagement::set_led_at(second_cell, 13);
    LEDManagement::set_led_at(third_cell, 20);
    LEDManagement::set_updated(true);
  }

  void resetCounter() {
    counter = 9;
  }
 public:
  uint8_t counter = 3;
 private:
  static constexpr RGBW red    = {255, 0, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};
#endif
};

static LedModeSerializable_LowBattery ledModeSerializableLowBattery{CRC32_STR("LedModeSerializable_LowBattery")};
