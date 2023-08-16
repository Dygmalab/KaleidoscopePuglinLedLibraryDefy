#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#include "debug_print.h"
#include <vector>
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
    output[++index] = paired_channel_id_;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index          = LedModeSerializable::deSerialize(input);
    paired_channels_                     = input[index];
    connected_channel_id_                     = input[++index];
    paired_channel_id_                     = input[++index];
    base_settings.delay_ms = 100;
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
    for (int i = 7; i >= 0; i--) { // Iterar a través de cada bit
      bool bit = ( paired_channels_>> i) & 1; // Leer el bit en la posición i usando desplazamiento y AND

      if (bit) {
        key_color[i] = white;
        is_paired[i] = 1;
      } else {
        key_color[i] = blue;
        is_paired[i] = 0;
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
    }
    LEDManagement::set_updated(true);
  }
#endif
  uint8_t paired_channels_;
  uint8_t connected_channel_id_;
  uint8_t paired_channel_id_;

 private:
  static constexpr RGBW white  = {0, 0, 0, 255};
  static constexpr RGBW green  = {0, 255, 0, 0};
  static constexpr RGBW blue  = {0, 0, 255, 0};
  static constexpr RGBW red    = {255, 0, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};
  enum Channels : uint8_t {
    NOT_CONNECTED = 5
  };
  std::vector<RGBW> key_color{5};
  std::vector<uint8_t> is_paired{5};
};

static LedModeSerializable_BluetoothPairing
  ledModeSerializableBluetoothPairing{CRC32_STR("LedModeSerializable_BluetoothPairing")};