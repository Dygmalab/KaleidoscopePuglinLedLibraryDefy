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

#include "LEDEffect-Bluetooth-Pairing-Defy.h"

namespace kaleidoscope {
namespace plugin {

void LEDBluetoothPairingDefy::TransientLEDMode::onActivate(void) {
  sendLedMode(parent_->led_mode);
}

void LEDBluetoothPairingDefy::TransientLEDMode::update(void) {
  parent_->led_mode.update();
}
void LEDBluetoothPairingDefy::setPairedChannels(uint8_t channel) {
  ledModeSerializableBluetoothPairing.paired_channels_ = channel;
}
void LEDBluetoothPairingDefy::setConnectedChannel(uint8_t channel) {
  ledModeSerializableBluetoothPairing.connected_channel_id_ = channel;
}
}  // namespace plugin
}  // namespace kaleidoscope
kaleidoscope::plugin::LEDBluetoothPairingDefy ledBluetoothPairingDefy;