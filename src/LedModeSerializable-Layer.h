#pragma once

#include "LedModeSerializable.h"
#include "cstdio"
#ifdef KEYSCANNER
#include "debug_print.h"
#include "BatteryManagement.hpp"
#endif

class LedModeSerializable_Layer : public LedModeSerializable {
 public:
  explicit LedModeSerializable_Layer(uint32_t id)
    : LedModeSerializable(id) {
  }
  uint8_t serialize(uint8_t *output) const override {
    uint8_t index = LedModeSerializable::serialize(output);
    output[index] = layer;
    output[++index] = fade_is_on;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index = LedModeSerializable::deSerialize(input);
    base_settings.delay_ms = 10;
    layer         = input[index];
    fade_is_on    = input[++index];
    return ++index;
  }
#ifdef NEURON_WIRED
  void update() override {
  }
#endif

#ifdef KEYSCANNER
  void update() override {
    static float led_driver_brightness = LEDManagement::get_max_ledDriver_brightness();
    static float underglow_brightness = LEDManagement::get_max_underglow_brightness();
    static bool max_reached = false;
    constexpr float top_brightness_level = 0.7f;
    constexpr float top_ug_brightness_level = 0.29f;
    static bool reached_ug_brightness = false;
    static bool reached_bl_brightness = false;

    DBG_PRINTF_TRACE("RECEIVE fade_is_on: %i", fade_is_on);

    float min_led_driver_brightness;
    float min_underglow_brightness;

    if (BatteryManagement::brightness_bl!=0){
      min_led_driver_brightness = BatteryManagement::brightness_bl/100;
      min_underglow_brightness = BatteryManagement::brightness_ug/100;
    } else{
      min_led_driver_brightness = LEDManagement::get_max_ledDriver_brightness();
      min_underglow_brightness = LEDManagement::get_max_underglow_brightness();
    }
    DBG_PRINTF_TRACE("min_led_driver_brightness: %f", min_led_driver_brightness);

    LEDManagement::Layer actualLayer{};
    if (this->layer < LEDManagement::layers.size()) {
      actualLayer = LEDManagement::layers.at(this->layer);
    }

    if (layer != previous_layer) {
      DBG_PRINTF_TRACE("layer: %i", layer);
      DBG_PRINTF_TRACE("previous_layer: %i", previous_layer);
      previous_layer = layer;
    }

    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++) {
      RGBW &color = LEDManagement::palette[actualLayer.leds[i]];
      LEDManagement::set_led_at(color, i);
    }

    if (layer != 0) {
      // Denied the brightness control to the battery management for a smooth transition in the fade effect.
      BatteryManagement::brightnessHandler(false);

      if (!max_reached) {
        underglow_brightness = std::min(top_ug_brightness_level, underglow_brightness + 0.004f);
        led_driver_brightness = std::min(top_brightness_level, led_driver_brightness + 0.06f);

        if (led_driver_brightness >= top_brightness_level) {
          max_reached = true;
        }
      } else {
        led_driver_brightness = std::max(min_led_driver_brightness, led_driver_brightness - 0.01002f);
        underglow_brightness = std::max(min_underglow_brightness, underglow_brightness - 0.00105f);

        if (led_driver_brightness <= min_led_driver_brightness) {
          reached_bl_brightness = true;
        }
        if (underglow_brightness <= min_underglow_brightness) {
          reached_ug_brightness = true;
        }
        if (reached_ug_brightness && reached_bl_brightness) {
          base_settings.delay_ms = 0;
          reached_ug_brightness = reached_bl_brightness = false;
          BatteryManagement::brightnessHandler(true);
        }
      }
    } else if (layer == 0) {
      max_reached = false;
      led_driver_brightness = min_led_driver_brightness;
      underglow_brightness = min_underglow_brightness;
      reached_ug_brightness = reached_bl_brightness = false;

      // Allow battery management to control the battery if it's low.
      BatteryManagement::brightnessHandler(true);
      base_settings.delay_ms = 0;
    }

    DBG_PRINTF_TRACE("led_driver_brightness: %f", led_driver_brightness);
    LEDManagement::set_ledDriver_brightness(led_driver_brightness);
    LEDManagement::set_underglow_brightness(underglow_brightness);
    LEDManagement::set_updated(true);
  }

#endif
  uint8_t layer;
  uint8_t fade_is_on = false;

 private:
};

static LedModeSerializable_Layer ledModeSerializableLayer{CRC32_STR("LedModeSerializable_Layer")};