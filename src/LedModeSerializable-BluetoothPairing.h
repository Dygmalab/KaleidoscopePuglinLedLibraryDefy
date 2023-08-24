#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#include "debug_print.h"
#include <vector>
#include "LedModeSerializable-Breathe.h"
#endif

class LedModeSerializable_BluetoothPairing : public LedModeSerializable {
 public:
  explicit LedModeSerializable_BluetoothPairing(uint32_t id)
    : LedModeSerializable(id) {
  }
  uint8_t serialize(uint8_t *output) const override {
    uint8_t index   = LedModeSerializable::serialize(output);
    output[index]   = paired_channels_;
    output[++index] = connected_channel_id_;
    output[++index] = advertising_id;
    output[++index] = defy_id_side;
    output[++index] = erease_done;

    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index          = LedModeSerializable::deSerialize(input);
    paired_channels_                     = input[index];
    connected_channel_id_                     = input[++index];
    advertising_id                     = input[++index];
    defy_id_side                     = input[++index];
    erease_done                     = input[++index];
    base_settings.delay_ms = 10;
    return ++index;
  }

#ifdef NEURON_WIRED
  void update() override {
  }
#endif

#ifdef KEYSCANNER
  void update() override {
    DBG_PRINTF_TRACE("PAIRED CHANNELD IDS %i", paired_channels_);
    DBG_PRINTF_TRACE("CONNECTED CHANNEL ID %i", connected_channel_id_);
    DBG_PRINTF_TRACE("ADVERSITING MODE: %i", advertising_id);
    for (int i = 4; i >= 0; i--) { // Iterar a través de cada bit
      bool bit = ( paired_channels_>> i) & 1; // Leer el bit en la posición i usando desplazamiento y AND

      if (bit) {
        key_color[i+1] = white;
        is_paired[i+1] = 1;
      } else {
        key_color[i+1] = blue;
        is_paired[i+1] = 0;
      }
    }
    for (uint8_t i = 1; i < 6 ; ++i) {
      LEDManagement::set_led_at(key_color[i], i);
      if (is_paired[i] == 1){
        LEDManagement::set_led_at(red, i+7);
      } else{
        LEDManagement::set_led_at(ledOff, i+7);
      }
    }
    if (connected_channel_id_ != NOT_CONNECTED && connected_channel_id_ < 5){
      LEDManagement::set_led_at(green, connected_channel_id_ + 1);
      LEDManagement::set_led_at(red, connected_channel_id_ + 8);
    }
    if(advertising_id != NOT_ON_ADVERTISING){
      breathe(advertising_id);
    }
    LEDManagement::set_updated(true);
  }

  void breathe(uint8_t channel_id){
    static uint32_t lastExecutionTime  = 0;
    uint8_t i = ((uint16_t)to_ms_since_boot(get_absolute_time())) >> 4;

    if (i & 0x80) {
      i = 255 - i;
    }

    i           = i << 1;
    uint8_t ii  = (i * i) >> 8;
    uint8_t iii = (ii * i) >> 8;

    i = (((3 * (uint16_t)(ii)) - (2 * (uint16_t)(iii))) / 2) + 80;

    RGBW breathe = LEDManagement::HSVtoRGB(160, 255, i);
    breathe.w    = 0;
    LEDManagement::set_led_at(breathe, channel_id + 1);
    if(underglow_led_id > 88){
      underglow_led_id = 35;
/*      for (int j = 35; j < 88; ++j) {
        LEDManagement::set_led_at(ledOff, j);
      }*/
    }
    LEDManagement::set_led_at(breathe, underglow_led_id);
    if ((uint16_t)to_ms_since_boot(get_absolute_time()) - lastExecutionTime >= 30){
      lastExecutionTime = (uint16_t)to_ms_since_boot(get_absolute_time());
      if(underglow_led_id - 1 != 34){
        LEDManagement::set_led_at(ledOff, underglow_led_id - 1);
      }
      if (underglow_led_id + 1 > 88){
        LEDManagement::set_led_at(blue, 35);
      } else {
        LEDManagement::set_led_at(blue, underglow_led_id + 1);
      }
      underglow_led_id++;
    }
  }
#endif
  uint8_t paired_channels_;
  uint8_t connected_channel_id_;
  uint8_t advertising_id;
  uint8_t erease_done;
  uint8_t defy_id_side;

 private:
#ifdef KEYSCANNER
  static constexpr RGBW white  = {0, 0, 0, 255};
  static constexpr RGBW green  = {0, 255, 0, 0};
  static constexpr RGBW blue  = {0, 0, 255, 0};
  static constexpr RGBW red    = {255, 0, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};
  enum Channels : uint8_t {
    NOT_CONNECTED = 5,
    NOT_ON_ADVERTISING
  };
  std::vector<RGBW> key_color{5};
  std::vector<uint8_t> is_paired{5};
  uint8_t underglow_led_id = 35;
#endif
};

static LedModeSerializable_BluetoothPairing
  ledModeSerializableBluetoothPairing{CRC32_STR("LedModeSerializable_BluetoothPairing")};