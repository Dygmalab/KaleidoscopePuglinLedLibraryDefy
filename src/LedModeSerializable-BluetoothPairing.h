#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#include "debug_print.h"
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
        // Si el bit está en 1, hacer algo (por ejemplo, imprimir un mensaje)
        DBG_PRINTF_TRACE("BIT %i EN 1", i);
      } else {
        // Si el bit está en 0, hacer algo diferente (por ejemplo, imprimir un mensaje diferente)
        DBG_PRINTF_TRACE("BIT %i EN 0", i);
      }
    }
    for (uint8_t i = 1; i < 6 ; ++i) {
      LEDManagement::set_led_at(blue, i);
    }
    for (uint8_t i = 8; i < 13 ; ++i) {
      LEDManagement::set_led_at(red, i);
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
};

static LedModeSerializable_BluetoothPairing
  ledModeSerializableBluetoothPairing{CRC32_STR("LedModeSerializable_BluetoothPairing")};