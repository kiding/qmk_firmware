/* Copyright 2021 Brandon Lewis
  * 
  * This program is free software: you can redistribute it and/or modify 
  * it under the terms of the GNU General Public License as published by 
  * the Free Software Foundation, either version 2 of the License, or 
  * (at your option) any later version. 
  * 
  * This program is distributed in the hope that it will be useful, 
  * but WITHOUT ANY WARRANTY; without even the implied warranty of 
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
  * GNU General Public License for more details. 
  * 
  * You should have received a copy of the GNU General Public License 
  * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
  */

#include "fruitbar.h"

_S_ROTARY rotary_state = _S_VOLUME;
uint32_t num_timer = 0;
uint32_t fn_timer = 0;

void _alt_start(uint8_t layer, _S_ROTARY new_state) {
  layer_on(layer);

  rotary_state = new_state;
}

void _alt_end(void) {
  layer_off(_L_NUM);
  layer_off(_L_FN);

  rotary_state = _S_VOLUME;
}
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  layer_state_t highest_layer = get_highest_layer(layer_state);

  switch (keycode) {
    case _KC_ROTARY:
      if (record->event.pressed) {
        switch (rotary_state) {
          case _S_VOLUME:
            _alt_start(highest_layer, _S_BRIGHTNESS);
            break;
          case _S_BRIGHTNESS:
          default:
            _alt_end();
        }
      }
      return false;
    case _KC_NUM:
      if (record->event.pressed) {
        _alt_start(_L_NUM, rotary_state);
      } else {
        if (timer_elapsed32(num_timer) > 250) {
          _alt_end();
        }
        num_timer = timer_read32();
      }
      return false;
    case _KC_FN:
      if (record->event.pressed) {
        _alt_start(_L_FN, rotary_state);
      } else {
        if (timer_elapsed32(fn_timer) > 250) {
          _alt_end();
        }
        fn_timer = timer_read32();
      }
      return false;
    default:
      return true; // Process all other keycodes normally
  }
}

bool encoder_update_user(uint8_t index, bool clockwise) {
  layer_state_t highest_layer = get_highest_layer(layer_state);
  clockwise = !clockwise; // It's inverted for some reason

  switch (rotary_state) {
    case _S_VOLUME:
      tap_code_delay(clockwise ? KC_KB_VOLUME_UP : KC_KB_VOLUME_DOWN, 10);
      return false;
    case _S_BRIGHTNESS:
    default:
      tap_code_delay(clockwise ? KC_BRIGHTNESS_UP : KC_BRIGHTNESS_DOWN, 10);
      _alt_start(highest_layer, rotary_state);
      return false;
  }
}

uint32_t oled_timer = 0;
const uint32_t OLED_FRAME_RATE = 50;

bool oled_task_user(void) {
  layer_state_t highest_layer = get_highest_layer(layer_state);

  if (timer_elapsed32(oled_timer) <= OLED_FRAME_RATE) {
    return false;
  }

  switch (highest_layer) {
    case _L_BASE:
      oled_write_P(PSTR("           {}|B\n qwertyuiop[]\\B\n"), false);
      break;
    case _L_NUM:
      oled_write_P(PSTR("~!@#$%^&*()_+ B\n`1234567890-= B\n"), false);
      break;
    case _L_FN:
      oled_write_P(PSTR("\nE123456789012 D\n"), false);
      break;
    default:
      oled_write_P(PSTR("HUH\n"), false);
  }

  switch (rotary_state) {
    case _S_VOLUME:
      oled_write_P(PSTR("               Volume\n"), false);
      break;
    case _S_BRIGHTNESS:
      oled_write_P(PSTR("           Brightness\n"), false);
      break;
    default:
      oled_write_P(PSTR("HUH\n"), false);
  }

  oled_timer = timer_read32();

  return false;
}
