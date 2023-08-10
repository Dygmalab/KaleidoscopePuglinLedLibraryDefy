/* -*- mode: c++ -*-
* Kaleidoscope-Colormap -- Per-layer colormap effect
* Copyright (C) 2016, 2017, 2018  Keyboard.io, Inc
*
* This program is free software: you can redistribute it and/or modify it under it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation, version 3.
*
* This program is distributed in the hope that it will be useful, but WITHOUT but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with along with
* this program. If not, see <http://www.gnu.org/licenses/>.
*/


#include "Colormap-Defy.h"
#include <Kaleidoscope-EEPROM-Settings.h>
#include <Kaleidoscope-FocusSerial.h>
#include "kaleidoscope/layers.h"
#include "LED-Palette-Theme-Defy.h"

namespace kaleidoscope {
namespace plugin {

uint16_t ColormapEffectDefy::map_base_;
uint8_t ColormapEffectDefy::max_layers_;
uint8_t ColormapEffectDefy::top_layer_;
ColormapEffectDefy::Side sides_colormap{0};
ColormapEffectDefy::DeviceID deviceId{Communications_protocol::KEYSCANNER_DEFY_RIGHT, Communications_protocol::KEYSCANNER_DEFY_LEFT};
void ColormapEffectDefy::max_layers(uint8_t max_) {
  if (map_base_ != 0)
    return;

  max_layers_ = max_;
  map_base_   = ::LEDPaletteThemeDefy.reserveThemes(max_layers_);
}

void ColormapEffectDefy::TransientLEDMode::onActivate(void) {
  if (!Runtime.has_leds)
    return;

  parent_->top_layer_     = Layer.mostRecent();
  parent_->led_mode.layer = parent_->top_layer_;
  sendLedMode(parent_->led_mode);
  if (parent_->top_layer_ <= parent_->max_layers_)
    ::LEDPaletteThemeDefy.updateHandler(parent_->map_base_, parent_->top_layer_);
}


void ColormapEffectDefy::updateColorIndexAtPosition(uint8_t layer, uint16_t position, uint8_t palette_index) {
  if (layer >= max_layers_) return;

  uint16_t index = (Runtime.device().led_count) * layer + position;
  ::LEDPaletteThemeDefy.updateColorIndexAtPosition(map_base_, index, palette_index);
}

uint8_t ColormapEffectDefy::getColorIndexAtPosition(uint8_t layer, uint16_t position) {
  if (layer >= max_layers_) return 0;

  uint16_t index = (Runtime.device().led_count) * layer + position;
  return ::LEDPaletteThemeDefy.lookupColorIndexAtPosition(map_base_, index);
}

void ColormapEffectDefy::TransientLEDMode::refreshAt(KeyAddr key_addr) {
  if (parent_->top_layer_ <= parent_->max_layers_)
    ::LEDPaletteThemeDefy.refreshAt(parent_->map_base_, parent_->top_layer_, key_addr);
}

EventHandlerResult ColormapEffectDefy::onLayerChange() {
  if (::LEDControl.get_mode_index() == led_mode_id_)
    ::LEDControl.get_mode<TransientLEDMode>()->onActivate();
  return EventHandlerResult::OK;
}

EventHandlerResult ColormapEffectDefy::onFocusEvent(const char *command) {
  const char *expected_command = "colormap.map";

  if (!Runtime.has_leds)
    return EventHandlerResult::OK;

  if (::Focus.handleHelp(command, expected_command))
    return EventHandlerResult::OK;

  if (strcmp(command, expected_command) != 0)
    return EventHandlerResult::OK;

  uint16_t max_index = (max_layers_ * (Runtime.device().led_count / 2));

  if (::Focus.isEOL()) {
    for (uint16_t pos = 0; pos < max_index; pos++) {
      uint8_t indexes = Runtime.storage().read(map_base_ + pos);

      ::Focus.send((uint8_t)(indexes >> 4), indexes & ~0xf0);
    }
    return EventHandlerResult::EVENT_CONSUMED;
  }

  uint16_t pos = 0;

  while (!::Focus.isEOL() && (pos < max_index)) {
    uint8_t idx1, idx2;
    ::Focus.read(idx1);
    ::Focus.read(idx2);

    uint8_t indexes = (idx1 << 4) + idx2;

    Runtime.storage().update(map_base_ + pos, indexes);
    pos++;
  }

  Runtime.storage().commit();


  ::LEDControl.refreshAll();
  return EventHandlerResult::EVENT_CONSUMED;
}

void ColormapEffectDefy::getLayer(uint8_t layer, uint8_t output_buf[Runtime.device().led_count]) {
  for (int i = 0; i < Runtime.device().led_count; ++i) {
    output_buf[i] = getColorIndexAtPosition(layer, i);
  }
}

uint8_t ColormapEffectDefy::getMaxLayers() {
  return max_layers_;
}

EventHandlerResult ColormapEffectDefy::onSetup() {
  Communications.callbacks.bind(PALETTE_COLORS, ([](Packet packet) { LEDPaletteThemeDefy::updatePaletteCommunication(packet); }));
  Communications.callbacks.bind(MODE_LED, ([](Packet packet) { ::LEDControl.set_mode(::LEDControl.get_mode_index()); }));
  Communications.callbacks.bind(LAYER_KEYMAP_COLORS, ([this](Packet packet) {
                                  updateKeyMapCommunications(packet);
                                }));
  Communications.callbacks.bind(LAYER_UNDERGLOW_COLORS, ([this](Packet packet) {
                                  updateUnderGlowCommunications(packet);
                                }));
  Communications.callbacks.bind(BRIGHTNESS, ([](Packet packet) {
                                  packet.header.command = BRIGHTNESS;
                                  packet.header.size    = 2;

                                  if (sides_colormap.rigth_side_connected && sides_colormap.left_side_connected) {
                                    packet.header.device = Communications_protocol::KEYSCANNER_DEFY_LEFT;
                                    packet.data[0]       = Runtime.device().ledDriver().getBrightness();
                                    packet.data[1]       = Runtime.device().ledDriver().getBrightnessUG();
                                    Communications.sendPacket(packet);
                                    packet.header.device = Communications_protocol::KEYSCANNER_DEFY_RIGHT;
                                    Communications.sendPacket(packet);
                                    NRF_LOG_DEBUG("BOTH DEVICES CONNECTED");
                                    NRF_LOG_DEBUG("BRIGHTNESS SENT: %i", packet.data[0]);
                                  } else if (!sides_colormap.rigth_side_connected && sides_colormap.left_side_connected) {
                                    packet.header.device = Communications_protocol::RF_DEFY_RIGHT;
                                    packet.data[0]       = Runtime.device().ledDriver().getBrightnessWireless();
                                    packet.data[1]       = Runtime.device().ledDriver().getBrightnessUGWireless();
                                    Communications.sendPacket(packet);
                                    packet.header.device = Communications_protocol::KEYSCANNER_DEFY_LEFT;
                                    Communications.sendPacket(packet);
                                    NRF_LOG_DEBUG("RIGHT SIDE DISCONNECTED");
                                    NRF_LOG_DEBUG("LEFT SIDE CONNECTED");
                                    NRF_LOG_DEBUG("BRIGHTNESS SENT: %i", packet.data[0]);
                                  } else if(sides_colormap.rigth_side_connected && !sides_colormap.left_side_connected){
                                    packet.header.device = Communications_protocol::KEYSCANNER_DEFY_RIGHT;
                                    packet.data[0]       = Runtime.device().ledDriver().getBrightnessWireless();
                                    packet.data[1]       = Runtime.device().ledDriver().getBrightnessUGWireless();
                                    Communications.sendPacket(packet);
                                    packet.header.device = Communications_protocol::RF_DEFY_LEFT;
                                    Communications.sendPacket(packet);
                                    NRF_LOG_DEBUG("RIGHT SIDE CONNECTED");
                                    NRF_LOG_DEBUG("LEFT SIDE DISCONNECTED");
                                    NRF_LOG_DEBUG("BRIGHTNESS SENT: %i", packet.data[0]);
                                  } else if(!sides_colormap.rigth_side_connected && !sides_colormap.left_side_connected){
                                    packet.header.device = Communications_protocol::RF_DEFY_RIGHT;
                                    packet.data[0]       = Runtime.device().ledDriver().getBrightnessWireless();
                                    packet.data[1]       = Runtime.device().ledDriver().getBrightnessUGWireless();
                                    Communications.sendPacket(packet);
                                    packet.header.device = Communications_protocol::RF_DEFY_LEFT;
                                    Communications.sendPacket(packet);
                                    NRF_LOG_DEBUG("SIDES DISCONNECTED");
                                    NRF_LOG_DEBUG("BRIGHTNESS SENT: %i", packet.data[0]);
                                  }
                                  Runtime.device().ledDriver().ConnectionStatus(sides_colormap.rigth_side_connected, sides_colormap.left_side_connected);
                                  NRF_LOG_FLUSH();
                                }));
  return EventHandlerResult::OK;
}

void ColormapEffectDefy::setSideStatus(Communications_protocol::Devices side) {

  if (side == Communications_protocol::KEYSCANNER_DEFY_RIGHT) {
    deviceId.right = side;
    sides_colormap.rigth_side_connected = true;
  }
  if (side == Communications_protocol::KEYSCANNER_DEFY_LEFT) {
    deviceId.left = side;
    sides_colormap.left_side_connected = true;
  }

  if (side == Communications_protocol::RF_DEFY_RIGHT || side == Communications_protocol::BLE_DEFY_RIGHT) {
    deviceId.right = side;
    sides_colormap.rigth_side_connected = false;
  }
  if (side == Communications_protocol::RF_DEFY_LEFT || side == Communications_protocol::BLE_DEFY_LEFT) {
    deviceId.left = side;
    sides_colormap.left_side_connected = false;
  }
}

void ColormapEffectDefy::updateKeyMapCommunications(Packet &packet) {
  uint8_t layerColors[Runtime.device().led_count];
  uint8_t baseKeymapIndex;
  if (packet.header.device == KEYSCANNER_DEFY_RIGHT || packet.header.device == Communications_protocol::RF_DEFY_RIGHT || packet.header.device == Communications_protocol::BLE_DEFY_RIGHT) {
    baseKeymapIndex = Runtime.device().ledDriver().key_matrix_leds;
  } else {
    baseKeymapIndex = 0;
  }
  union PaletteJoiner {
    struct {
      uint8_t firstColor : 4;
      uint8_t secondColor : 4;
    };
    uint8_t paletteColor;
  };
  uint8_t layer = packet.data[0];
  getLayer(layer, layerColors);
  packet.header.command       = LAYER_KEYMAP_COLORS;
  const uint8_t sizeofMessage = Runtime.device().ledDriver().key_matrix_leds / 2.0 + 0.5;
  PaletteJoiner message[sizeofMessage];
  packet.header.size = sizeof(message) + 1;
  packet.data[0]     = layer;
  uint8_t k{};
  bool swap = true;
  for (int j = 0; j < Runtime.device().ledDriver().key_matrix_leds; ++j) {
    if (swap) {
      message[k].firstColor = layerColors[baseKeymapIndex + j];
    } else {
      message[k++].secondColor = layerColors[baseKeymapIndex + j];
    }
    swap = !swap;
  }
  memcpy(&packet.data[1], message, packet.header.size - 1);
  Communications.sendPacket(packet);
}


void ColormapEffectDefy::updateUnderGlowCommunications(Packet &packet) {
  uint8_t layerColors[Runtime.device().led_count];
  uint8_t baseUnderGlowIndex;
  if (packet.header.device == KEYSCANNER_DEFY_RIGHT || packet.header.device == Communications_protocol::RF_DEFY_RIGHT || packet.header.device == Communications_protocol::BLE_DEFY_RIGHT) {
    baseUnderGlowIndex = (Runtime.device().ledDriver().key_matrix_leds) * 2 +
                         Runtime.device().ledDriver().underglow_leds;
  } else {
    baseUnderGlowIndex = Runtime.device().ledDriver().key_matrix_leds * 2;
  }
  union PaletteJoiner {
    struct {
      uint8_t firstColor : 4;
      uint8_t secondColor : 4;
    };
    uint8_t paletteColor;
  };
  uint8_t layer = packet.data[0];
  getLayer(layer, layerColors);
  packet.header.command       = Communications_protocol::LAYER_UNDERGLOW_COLORS;
  const uint8_t sizeofMessage = Runtime.device().ledDriver().underglow_leds / 2.0 + 0.5;
  PaletteJoiner message[sizeofMessage];
  packet.header.size = sizeof(message) + 1;
  packet.data[0]     = layer;
  bool swap          = true;
  uint8_t k{};
  for (int j = 0; j < Runtime.device().ledDriver().underglow_leds; ++j) {
    if (swap) {
      message[k].firstColor = layerColors[baseUnderGlowIndex + j];
    } else {
      message[k++].secondColor = layerColors[baseUnderGlowIndex + j];
    }
    swap = !swap;
  }
  memcpy(&packet.data[1], message, packet.header.size - 1);
  Communications.sendPacket(packet);
}

}  // namespace plugin
}  // namespace kaleidoscope

kaleidoscope::plugin::ColormapEffectDefy ColormapEffectDefy;
