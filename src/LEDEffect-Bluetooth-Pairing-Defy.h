/* Kaleidoscope-LEDEffect-SolidColor - Solid color LED effects for Kaleidoscope.
 * Copyright (C) 2017  Keyboard.io, Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "Kaleidoscope-LEDControl.h"
#include "LedModeSerializable-BluetoothPairing.h"
#include "LedModeCommunication.h"

namespace kaleidoscope {
namespace plugin {
class LEDBluetoothPairingDefy : public Plugin,
                          public LEDModeInterface,
                          public LedModeCommunication {
 public:
  LEDBluetoothPairingDefy(){
    led_mode.base_settings.delay_ms = 50;
  }
  LedModeSerializable_BluetoothPairing &led_mode = ledModeSerializableBluetoothPairing;

  void setPairedChannels(uint8_t channel);

  void setConnectedChannel(uint8_t channel);

  void setAvertisingModeOn(uint8_t advertising_id_);
  // This class' instance has dynamic lifetime
  //
  class TransientLEDMode : public LEDMode {
   public:
    // Please note that storing the parent ptr is only required
    // for those LED modes that require access to
    // members of their parent class. Most LED modes can do without.
    //
    explicit TransientLEDMode(LEDBluetoothPairingDefy *parent)
      : parent_(parent) {}

    void update() final;

   protected:
    void onActivate(void) final;

   private:
    const LEDBluetoothPairingDefy *parent_;
    uint8_t paired_channels_;
  };
};
}  // namespace plugin
}  // namespace kaleidoscope
extern kaleidoscope::plugin::LEDBluetoothPairingDefy ledBluetoothPairingDefy;