#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#include "BatteryManagement.hpp"
#endif

class LedModeSerializable_BatteryStatus : public LedModeSerializable {
 public:
  explicit LedModeSerializable_BatteryStatus(uint32_t id)
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
    RGBW first_cell, second_cell, fourth_cell, third_cell = {0, 0, 0, 0};
    constexpr float top_brightness_level = 0.7f;
    constexpr float top_ug_brightness_level = 0.29f;
    BatteryManagement::brightnessHandler(false);
    LEDManagement::set_ledDriver_brightness(top_brightness_level);
    LEDManagement::set_underglow_brightness(top_ug_brightness_level);
    uint8_t batteryLevel = BatteryManagement::getBatteryLevel();
    if (batteryLevel > 70) {

      first_cell  = green;
      second_cell = green;
      third_cell  = green;
    } else if (batteryLevel > 40) {

      first_cell  = ledOff;
      second_cell = green;
      third_cell  = green;

    } else if (batteryLevel > 10) {
      first_cell  = ledOff;
      second_cell = ledOff;
      third_cell  = green;

    } else {
      first_cell  = ledOff;
      second_cell = ledOff;
      third_cell  = red;
    }

    /*Column effect*/
    if (BatteryManagement::getBatteryStatus() == BatteryManagement::CHARGING_DONE) {

      first_cell  = green;
      second_cell = green;
      third_cell  = green;

    } else if (BatteryManagement::getBatteryStatus() == BatteryManagement::CHARGING) {
      static enum {
        FIRST_CELL,
        SECOND_CELL,
        THIRD_CELL,
        NO_CELL,
      } currentCell = NO_CELL;
      switch (currentCell) {
      case NO_CELL:
        first_cell  = ledOff;
        second_cell = ledOff;
        third_cell  = ledOff;
        currentCell = THIRD_CELL;
        break;
      case THIRD_CELL:
        first_cell  = ledOff;
        second_cell = ledOff;
        third_cell  = green;
        currentCell = SECOND_CELL;
        break;
      case SECOND_CELL:
        first_cell  = ledOff;
        second_cell = green;
        third_cell  = green;
        currentCell = FIRST_CELL;
        break;
      case FIRST_CELL:
        first_cell  = green;
        second_cell = green;
        third_cell  = green;
        currentCell = NO_CELL;
        break;
      }
    }

    LEDManagement::set_led_at(first_cell, 6);
    LEDManagement::set_led_at(second_cell, 13);
    LEDManagement::set_led_at(third_cell, 20);
    LEDManagement::set_updated(true);
  }

 private:
  static constexpr RGBW green  = {0, 255, 0, 0};
  static constexpr RGBW yellow = {255, 255, 0, 0};
  static constexpr RGBW red    = {255, 0, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};

  RGBW ledToggle(RGBW ledColor) {
    static bool ledStatus = false;
    RGBW color            = ledOff;
    if (ledStatus) {
      color = ledColor;
    }
    ledStatus = !ledStatus;
    return color;
  }
#endif
};

static LedModeSerializable_BatteryStatus ledModeSerializableBatteryStatus{CRC32_STR("LedModeSerializable_BatteryStatus")};
