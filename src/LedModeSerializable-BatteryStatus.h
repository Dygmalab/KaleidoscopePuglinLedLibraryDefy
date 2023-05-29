#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
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

    RGBW ledColor_green, ledColor_yellow, ledColor_orange, ledColor_red = {0, 0, 0, 0};
    //TODO: Erase the breathe effect and orange color if it is not being used.
    //Set the battery status
    batteryStatus -= BatteryDrainFactor;
    if (batteryStatus < 0) {
      batteryStatus = 100;
    }

    if (batteryStatus > 90) {

      ledColor_green  = green;
      ledColor_yellow = green;
      ledColor_red    = green;
      //ledColor_orange = {0, 255, 0, 0};

    } else if (batteryStatus > 75) {

      //ledColor_green  = breathe(static_cast<uint16_t>(80));  //HSV Green values
      ledColor_green  = ledToggle(green);
      ledColor_yellow = green;
      ledColor_red    = green;
      //ledColor_orange = {0, 255, 0, 0};

    } else if (batteryStatus > 50) {

      ledColor_green  = ledOff;
      ledColor_yellow = yellow;
      ledColor_red    = yellow;
      //ledColor_orange = {255, 255, 0, 0};

    } else if (batteryStatus > 35) {

      //ledColor_yellow = breathe(static_cast<uint16_t>(40));  //HSV Yellow values
      ledColor_yellow = ledToggle(yellow);
      ledColor_red    = yellow;
      //ledColor_orange = {255, 255, 0, 0};

    } else if (batteryStatus > 15) {

      ledColor_yellow = ledOff;
      ledColor_red    = red;

    } else {
      ledColor_red = ledToggle(red);
    }

    /*Column effect*/
    LEDManagement::set_led_at(ledColor_green, 6);
    LEDManagement::set_led_at(ledColor_yellow, 13);
    LEDManagement::set_led_at(ledColor_red, 20);

    // /*Cool effect*/
    // LEDManagement::set_led_at(ledColor_green, 27);
    // LEDManagement::set_led_at(ledColor_green, 34);

    // LEDManagement::set_led_at(ledColor_yellow, 28);
    // LEDManagement::set_led_at(ledColor_yellow, 33);

    // LEDManagement::set_led_at(ledColor_orange, 29);
    // LEDManagement::set_led_at(ledColor_orange, 32);

    // LEDManagement::set_led_at(ledColor_red, 30);
    // LEDManagement::set_led_at(ledColor_red, 31);
    // LEDManagement::set_updated(true);
  }
#endif
  uint8_t r_, g_, b_, w_;

 private:
  float BatteryDrainFactor   = 0.3;
  float batteryStatus        = 100;
  uint16_t rainbowHue        = 0;
  uint16_t rainbowSaturation = 255;
  uint8_t rainbowLastUpdate  = 0;
  const RGBW green           = {0, 255, 0, 0};
  const RGBW yellow          = {255, 255, 0, 0};
  const RGBW red             = {255, 0, 0, 0};
  const RGBW ledOff          = {0, 0, 0, 0};

  RGBW ledToggle(RGBW ledColor) {
    static bool ledStatus = false;
    RGBW color            = ledOff;
    if (ledStatus) {
      color = ledColor;
    }
    ledStatus = !ledStatus;
    return color;
  }

  RGBW breathe(uint16_t breatheHue) {
    uint8_t i                  = ((uint16_t)to_ms_since_boot(get_absolute_time())) >> 4;
    uint16_t breatheSaturation = 255;

    if (i & 0x80) {
      i = 255 - i;
    }
    i           = i << 1;
    uint8_t ii  = (i * i) >> 8;
    uint8_t iii = (ii * i) >> 8;

    i            = (((3 * (uint16_t)(ii)) - (2 * (uint16_t)(iii))) / 2) + 80;
    RGBW breathe = LEDManagement::HSVtoRGB(breatheHue, breatheSaturation, i);
    breathe.w    = 0;
    return breathe;
  }
};

static LedModeSerializable_BatteryStatus ledModeSerializableBatteryStatus{CRC32_STR("LedModeSerializable_BatteryStatus")};
