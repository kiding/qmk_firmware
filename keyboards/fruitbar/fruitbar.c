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
#include "keymap.h"  // to get keymaps[][][]

uint32_t num_bspc_timer = 0;
_S_ROTARY rotary_state = _S_VOLUME;
uint32_t rotary_timer = 0;

bool _is_keycode_num_bspc(uint16_t keycode) {
  for(int i=_L_NUM_BSPC; i<=_L_NUM_BSPC; i++) {
    for(int j=0; j<MATRIX_ROWS; j++) {
      for(int k=0; k<MATRIX_COLS; k++) {
        if (keycode == pgm_read_word(&keymaps[i][j][k])) {
          return true;
        }
      }
    }
  }

  return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (num_bspc_timer > 0 && _is_keycode_num_bspc(keycode)) {
    num_bspc_timer = timer_read32();
  }

  switch (keycode) {
    case _KC_NUM:
      if (record->event.pressed) {
        layer_on(_L_NUM_BSPC);
        num_bspc_timer = 0;
      } else {
        num_bspc_timer = timer_read32();
      }
      return true;
    case KC_RSFT:
      if (record->event.pressed) {
        layer_on(_L_NUM_DEL);
      } else {
        layer_off(_L_NUM_DEL);
      }
      return true;
    case _KC_ROTARY:
      if (record->event.pressed) {
        switch (rotary_state) {
          case _S_VOLUME:
            rotary_state = _S_BRIGHTNESS;
            rotary_timer = timer_read32();
            break;
          case _S_BRIGHTNESS:
          default:
            rotary_state = _S_VOLUME;
            rotary_timer = 0;
        }
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
      break;
    case _S_BRIGHTNESS:
    default:
      tap_code_delay(clockwise ? KC_BRIGHTNESS_UP : KC_BRIGHTNESS_DOWN, 10);
      break;
  }

  rotary_timer = timer_read32();
  return false;
}

uint32_t oled_timer = 0;
const uint32_t OLED_FRAME_RATE = 50;
const uint32_t ROTARY_TIMEOUT = 5000;
const uint32_t NUM_BSPC_TIMEOUT = 250;

bool oled_task_user(void) {
  layer_state_t highest_layer = get_highest_layer(layer_state);

  if (timer_elapsed32(oled_timer) <= OLED_FRAME_RATE) {
    return false;
  }

  switch (highest_layer) {
    case _L_BASE:
      oled_write_P(PSTR("           {}| "), false);
      break;
    case _L_NUM_BSPC:
      oled_write_P(PSTR("~!@#$%^&*()_+ B"), false);
      break;
    case _L_NUM_DEL:
      oled_write_P(PSTR("~!@#$%^&*()_+ D"), false);
      break;
    default:
      oled_write_P(PSTR("XXXXXXXXXXXXXXX"), false);
  }

  oled_write_P(PSTR("     "), false);

  switch (rotary_state) {
    case _S_VOLUME:
      oled_write_P(PSTR("\x0E"), false);
      break;
    case _S_BRIGHTNESS:
      oled_write_P(PSTR("\x0F"), false);
      break;
    default:
      oled_write_P(PSTR("X"), false);
  }

  switch (highest_layer) {
    case _L_BASE:
      oled_write_P(PSTR("\x1Aqwertyuiop[]\\B"), false);
      break;
    case _L_NUM_BSPC:
      oled_write_P(PSTR("`1234567890-= B"), false);
      break;
    case _L_NUM_DEL:
      oled_write_P(PSTR("`1234567890-= D"), false);
      break;
    default:
      oled_write_P(PSTR("XXXXXXXXXXXXXXX"), false);
  }

  oled_advance_char();
  oled_write(get_u16_str(get_current_wpm(), ' '), false);

  const uint32_t num_bspc_diff = timer_elapsed32(num_bspc_timer);
  if (num_bspc_timer > 0 && NUM_BSPC_TIMEOUT < num_bspc_diff) {
    layer_off(_L_NUM_BSPC);
    num_bspc_timer = 0;
  }
  char num_bspc_countdown[15] = "              ";
  if (num_bspc_timer > 0) {
    const size_t max = sizeof(num_bspc_countdown) - 1;
    const size_t len = max - num_bspc_diff * max / NUM_BSPC_TIMEOUT;
    for (size_t i=0; i<max; i++) {
      num_bspc_countdown[i] = i < len ? '\x07' : ' ';
    }
  }
  oled_write(num_bspc_countdown, false);
  oled_advance_char();
  oled_advance_char();

  const uint32_t rotary_diff = timer_elapsed32(rotary_timer);
  if (rotary_timer > 0 && ROTARY_TIMEOUT < rotary_diff) {
    rotary_state = _S_VOLUME;
    rotary_timer = 0;
  }
  char rotary_countdown[6] = "     ";
  if (rotary_timer > 0) {
    const size_t max = sizeof(rotary_countdown) - 1;
    const size_t len = max - rotary_diff * max / ROTARY_TIMEOUT;
    for (size_t i=0; i<max; i++) {
      rotary_countdown[i] = i < len ? '\x07' : ' ';
    }
  }
  oled_write(rotary_countdown, false);

  oled_timer = timer_read32();

  return false;
}
