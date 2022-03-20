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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case _KC_ROTARY:
      if (record->event.pressed) {
        rotary_state = rotary_state == _S_VOLUME ? _S_BRIGHTNESS : _S_VOLUME;
      }
      return false;
    case _KC_NUM:
      if (record->event.pressed) {
        layer_on(_L_NUM);
      } else {
        if (timer_elapsed32(num_timer) > 250) {
          layer_off(_L_NUM);
          layer_off(_L_FN);
        }
        num_timer = timer_read32();
      }
      return false;
    case _KC_FN:
      if (record->event.pressed) {
        layer_on(_L_FN);
      } else {
        if (timer_elapsed32(fn_timer) > 250) {
          layer_off(_L_FN);
          layer_off(_L_NUM);
        }
        fn_timer = timer_read32();
      }
      return false;
    default:
      return true; // Process all other keycodes normally
  }
}

bool encoder_update_user(uint8_t index, bool clockwise) {
  clockwise = !clockwise; // It's inverted for some reason

  switch (rotary_state) {
    case _S_VOLUME:
      tap_code_delay(clockwise ? KC_KB_VOLUME_UP : KC_KB_VOLUME_DOWN, 10);
      return false;
    case _S_BRIGHTNESS:
      tap_code_delay(clockwise ? KC_BRIGHTNESS_UP : KC_BRIGHTNESS_DOWN, 10);
      return false;
    default:
      return false;
  }
}

bool oled_task_user(void) {
  switch (get_highest_layer(layer_state)) {
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

  return false;
}
