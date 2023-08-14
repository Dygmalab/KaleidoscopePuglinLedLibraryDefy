#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
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

#ifdef KEYSCANNER
  void update() override {
    LEDManagement::set_all_leds({255, 255, 255, 255});
    LEDManagement::set_updated(true);
  }
#endif
  uint8_t paired_channels_;
  uint8_t connected_channel_id_;
  uint8_t paired_channel_id_;

 private:
};

static LedModeSerializable_BluetoothPairing
  ledModeSerializableBluetoothPairing{CRC32_STR("LedModeSerializable_BluetoothPairing")};