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

uint32_t alt_timer = 0;
uint32_t numfn_timer = 0;
_S_ROTARY rotary_state = _S_SCROLL;

void _alt_start(uint8_t layer, _S_ROTARY new_state) {
  layer_on(layer);

  alt_timer = timer_read32();
  rotary_state = new_state;
}

void _alt_end(void) {
  layer_off(_L_NUM);
  layer_off(_L_FN);

  alt_timer = 0;
  rotary_state = _S_SCROLL;
}

bool _is_keycode_alt(uint16_t keycode) {
  for(int i=_L_NUM; i<=_L_FN; i++) {
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
  layer_state_t highest_layer = get_highest_layer(layer_state);

  if (highest_layer != _L_BASE && _is_keycode_alt(keycode)) {
    _alt_start(highest_layer, rotary_state);
  }

  switch (keycode) {
    case _KC_ROTARY:
      if (record->event.pressed) {
        switch (rotary_state) {
          case _S_SCROLL:
            _alt_start(highest_layer, _S_VOLUME);
            break;
          case _S_VOLUME:
            _alt_start(highest_layer, _S_BRIGHTNESS);
            break;
          case _S_BRIGHTNESS:
          default:
            _alt_end();
        }
      }
      return false;
    case _KC_NUMFN:
      if (record->event.pressed) {
        _alt_start(_L_NUM, rotary_state);
      } else {
        if (timer_elapsed32(numfn_timer) <= 250) {
          _alt_start(_L_FN, rotary_state);
        } else {
          _alt_end();
        }
        numfn_timer = timer_read32();
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
    case _S_SCROLL:
      tap_code_delay(clockwise ? KC_PAGE_DOWN : KC_PAGE_UP, 10);
      return false;
    case _S_VOLUME:
      tap_code_delay(clockwise ? KC_KB_VOLUME_UP : KC_KB_VOLUME_DOWN, 10);
      _alt_start(highest_layer, rotary_state);
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
const uint32_t ALT_TIMEOUT = 7000;

bool oled_task_user(void) {
  layer_state_t highest_layer = get_highest_layer(layer_state);

  if (timer_elapsed32(oled_timer) <= OLED_FRAME_RATE) {
    return false;
  }

  switch (highest_layer) {
    case _L_BASE:
      oled_write_P(PSTR("           {}|B"), false);
      break;
    case _L_NUM:
      oled_write_P(PSTR("~!@#$%^&*()_+ B"), false);
      break;
    case _L_FN:
      oled_write_P(PSTR("               "), false);
      break;
    default:
      oled_write_P(PSTR("XXXXXXXXXXXXXXX"), false);
  }

  oled_advance_char();
  oled_write_P(PSTR("WPM"), false);
  oled_advance_char();

  switch (rotary_state) {
    case _S_SCROLL:
      oled_write_P(PSTR("\x12"), false);
      break;
    case _S_VOLUME:
      oled_write_P(PSTR(" "), false);
      break;
    case _S_BRIGHTNESS:
      oled_write_P(PSTR(" "), false);
      break;
    default:
      oled_write_P(PSTR("X"), false);
  }

  switch (highest_layer) {
    case _L_BASE:
      oled_write_P(PSTR("\x1Aqwertyuiop[]\\B"), false);
      break;
    case _L_NUM:
      oled_write_P(PSTR("`1234567890-= B"), false);
      break;
    case _L_FN:
      oled_write_P(PSTR("E123456789012 D"), false);
      break;
    default:
      oled_write_P(PSTR("XXXXXXXXXXXXXXX"), false);
  }

  oled_advance_char();
  oled_write(get_u8_str(get_current_wpm(), ' '), false);
  oled_advance_char();

  switch (rotary_state) {
    case _S_SCROLL:
      oled_write_P(PSTR(" "), false);
      break;
    case _S_VOLUME:
      oled_write_P(PSTR("\x0E"), false);
      break;
    case _S_BRIGHTNESS:
      oled_write_P(PSTR("\x0F"), false);
      break;
    default:
      oled_write_P(PSTR("X"), false);
  }

  const uint32_t alt_diff = timer_elapsed32(alt_timer);
  if (ALT_TIMEOUT < alt_diff) {
    _alt_end();
  }

  char alt_countdown[22] = {0};
  if (alt_timer) {
    const size_t max = sizeof(alt_countdown) - 1;
    const size_t len = max - alt_diff * max / ALT_TIMEOUT;
    for (size_t i=0; i<max; i++) {
      alt_countdown[i] = i < len ? '\x07' : ' ';
    }
  }
  oled_write_ln(alt_countdown, false);

  oled_timer = timer_read32();

  return false;
}
