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
    static uint32_t lastExecutionTime = 0;
    static bool blinking = false;
    uint32_t currentTime = to_ms_since_boot(get_absolute_time());

    DBG_PRINTF_TRACE("counter: %i", counter);

    if (counter > 0) {
      if (currentTime - lastExecutionTime >= 73) {
        blinking = !blinking;

        if (blinking) {
          LEDManagement::set_all_leds(red);
          DBG_PRINTF_TRACE("PRENDIDOOOOO");
        } else {
          LEDManagement::set_all_leds(ledOff);
          DBG_PRINTF_TRACE("APAGADOOO");
          counter--;
        }

        lastExecutionTime = currentTime;
      }
    } else {
      LEDManagement::set_all_leds(ledOff);
      DBG_PRINTF_TRACE("APAGADOOO");
    }

    LEDManagement::set_updated(true);
  }

  void setCounter(uint8_t counter_) {
    counter = counter_;
  }
 public:
  uint8_t counter = 5;
 private:
  static constexpr RGBW red    = {255, 0, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};
#endif
};

static LedModeSerializable_LowBattery ledModeSerializableLowBattery{CRC32_STR("LedModeSerializable_LowBattery")};
