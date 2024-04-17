/* -*- mode: c++ -*-
 * Kaleidoscope-Colormap -- Per-layer colormap effect
 * Copyright (C) 2022  Keyboard.io, Inc
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

#include "Colormap-Defy.h"  // for Colormap
#include "DefaultColormap.h"

#include "Kaleidoscope-LEDControl.h"  // for LEDControl
#include <stdint.h>                   // for uint8_t, uint16_t

#include "kaleidoscope/KeyAddr.h"    // for KeyAddr
#include "kaleidoscope/Runtime.h"    // for Runtime, Runtime_
#include "LED-Palette-Theme-Defy.h"  // for LEDPaletteTheme

#include "DefyFirmwareVersion.h"

namespace kaleidoscope {
namespace plugin {

const cRGB defaultcolormap::palette[16]{
  {255, 0, 0, 0},
  {0, 255, 0, 0},
  {87, 164, 255, 0},
  {0, 0, 0, 255},
  {150, 75, 0x00, 0},
  {0xff, 0x55, 0xff, 0},
  {255, 196, 0, 0},
  {0, 255, 234, 0},
  {0, 52, 255, 0},
  {255, 0, 232, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
};

enum {
  RED,
  GREEN,
  BLUE,
  WHITE,
  BROWN,
  MAGENT,
  YELLOW,
  LIGHT_BLUE,
  VIOLET,
  PINK,
  BLACK
};
bool defaultcolormap::palette_defined                                                = true;
const uint8_t defaultcolormap::colormap_layers                                       = 4;
const uint8_t defaultcolormap::colormaps_iso[1][kaleidoscope_internal::device.led_count] = {
  {//KeyMap Left
   YELLOW,
   LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
   GREEN,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
   GREEN,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
   GREEN,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
   PINK,
   BLUE,
    LIGHT_BLUE,
   GREEN,
    GREEN,
   RED,
   WHITE,
    //KeyMap Right
   RED,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    YELLOW,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
   GREEN,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
   YELLOW,
    YELLOW,
    YELLOW,
    YELLOW,
    GREEN,
    GREEN,
    RED,
   WHITE,
    //Underblow left
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    //Underglow right
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
  }
};
const uint8_t defaultcolormap::colormaps_ansi[1][kaleidoscope_internal::device.led_count] = {
  {//KeyMap Left
    YELLOW,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    GREEN,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    GREEN,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    GREEN,
    BLACK,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    PINK,
    BLUE,
    LIGHT_BLUE,
    GREEN,
    GREEN,
    RED,
    WHITE,
    //KeyMap Right
    RED,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    YELLOW,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    GREEN,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    LIGHT_BLUE,
    YELLOW,
    YELLOW,
    YELLOW,
    YELLOW,
    GREEN,
    GREEN,
    RED,
    WHITE,
    //Underblow left
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    //Underglow right
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
    WHITE,
  }
};
//alsdkfajñ
void DefaultColormap::setup() {
  install();
}

void DefaultColormap::install() {

  if (!defaultcolormap::palette_defined)
    return;

  for (uint8_t i = 0; i < 16; i++) {
    cRGB color;

    color.r = defaultcolormap::palette[i].r;
    color.g = defaultcolormap::palette[i].g;
    color.b = defaultcolormap::palette[i].b;
    color.w = defaultcolormap::palette[i].w;

    ::LEDPaletteThemeDefy.updatePaletteColor(i, color);
  }

  if (defaultcolormap::colormap_layers == 0)
    return;

  for (uint8_t layer = 0; layer < defaultcolormap::colormap_layers; layer++) {
    for (int16_t i = 0; i < Runtime.device().led_count; i++) {
      if (FirmwareVersion::get_layout() == FirmwareVersion::Device::ISO){
        const uint8_t idx = pgm_read_byte(&(defaultcolormap::colormaps_iso[layer][i]));
        ::ColormapEffectDefy.updateColorIndexAtPosition(layer, i, idx);
      } else {
        const uint8_t idx = pgm_read_byte(&(defaultcolormap::colormaps_ansi[layer][i]));
        ::ColormapEffectDefy.updateColorIndexAtPosition(layer, i, idx);
      }
    }
  }
  Runtime.storage().commit();

  ::LEDControl.refreshAll();
}

}  // namespace plugin
}  // namespace kaleidoscope

kaleidoscope::plugin::DefaultColormap DefaultColormap;