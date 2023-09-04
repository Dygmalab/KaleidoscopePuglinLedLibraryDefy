#pragma once

#include "LedModeSerializable.h"
#include "cstdio"
#include "debug_print.h"

class LedModeSerializable_Layer : public LedModeSerializable {
 public:
  explicit LedModeSerializable_Layer(uint32_t id)
    : LedModeSerializable(id) {
  }
  uint8_t serialize(uint8_t *output) const override {
    uint8_t index = LedModeSerializable::serialize(output);
    output[index] = layer;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index = LedModeSerializable::deSerialize(input);
    base_settings.delay_ms = 100;
    layer         = input[index];
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
    static bool flag = true;
    static RGBW aux_pallete;

    LEDManagement::Layer actualLayer{};
    if (this->layer < LEDManagement::layers.size()) {
      actualLayer = LEDManagement::layers.at(this->layer);
    }
    if (layer != previous_layer){
      DBG_PRINTF_TRACE("layer: %i", layer);
      DBG_PRINTF_TRACE("previous_layer: %i", previous_layer);
      previous_layer = layer;
    }
    //TODO: Fijate que el maximo brillo para el BL y el UG son distintos
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++) {
      RGBW &color = LEDManagement::palette[actualLayer.leds[i]];
      LEDManagement::set_led_at(color, i);
    }
    if (layer != 0){
      led_driver_brightness = led_driver_brightness - 0.02;
      underglow_brightness = underglow_brightness -  0.03;
    } else {
      led_driver_brightness = led_driver_brightness + 0.02;
      underglow_brightness = underglow_brightness  + 0.03;
    }
    DBG_PRINTF_TRACE("led_driver_brightness: %f", led_driver_brightness);
    DBG_PRINTF_TRACE("underglow_brightness: %f", underglow_brightness);
    if (led_driver_brightness <= 0.10){
      base_settings.delay_ms = 0;
    } else if (led_driver_brightness > LEDManagement::get_max_ledDriver_brightness()){
      led_driver_brightness  = LEDManagement::get_max_ledDriver_brightness();
      underglow_brightness = LEDManagement::get_max_underglow_brightness();
      base_settings.delay_ms = 0;
    }
    LEDManagement::set_ledDriver_brightness(led_driver_brightness);
    LEDManagement::set_underglow_brightness(led_driver_brightness);
    LEDManagement::set_updated(true);
  }
#endif
  uint8_t layer;
  uint8_t previous_layer = 0xff;

 private:
};

static LedModeSerializable_Layer ledModeSerializableLayer{CRC32_STR("LedModeSerializable_Layer")};