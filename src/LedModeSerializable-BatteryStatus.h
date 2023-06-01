#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#include "RFGW_communications.h"
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

    RGBW first_cell, second_cell, third_cell, status_led = {0, 0, 0, 0};

    //TODO: Move the 4.2 to a const
    uint8_t batteryLevel = RFGWCommunication::getBatteryLevel() / 42;
    printf("%f %i\n", batteryLevel, RFGWCommunication::getBatteryLevel());
    if (batteryLevel > 90) {

      first_cell  = green;
      second_cell = green;
      third_cell  = green;
    } else if (batteryLevel > 75) {

      first_cell  = ledToggle(green);
      second_cell = green;
      third_cell  = green;

    } else if (batteryLevel > 50) {

      first_cell  = ledOff;
      second_cell = yellow;
      third_cell  = yellow;

    } else if (batteryLevel > 35) {

      first_cell  = ledOff;
      second_cell = ledToggle(yellow);
      third_cell  = yellow;

    } else if (batteryLevel > 15) {
      first_cell  = ledOff;
      second_cell = ledOff;
      third_cell  = red;

    } else {
      first_cell  = ledOff;
      second_cell = ledOff;
      third_cell  = ledToggle(red);
    }

    /*Column effect*/
    if (RFGWCommunication::getBatteryStatus() == RFGWCommunication::CHARGING) {
      status_led = yellow;
    } else if (RFGWCommunication::getBatteryStatus() == RFGWCommunication::CHARGING_DONE) {
      status_led = green;
    }

    LEDManagement::set_led_at(status_led, 0);
    LEDManagement::set_led_at(first_cell, 6);
    LEDManagement::set_led_at(second_cell, 13);
    LEDManagement::set_led_at(third_cell, 20);
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
