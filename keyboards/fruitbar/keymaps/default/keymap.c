#include QMK_KEYBOARD_H
#include "fruitbar.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_L_BASE] = LAYOUT(
    KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,      KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,      KC_LBRC,   KC_RBRC,   KC_BSLS,   KC_BSPC,   _KC_ROTARY,
    KC_CAPS,   KC_A,      KC_S,      KC_D,      KC_F,      KC_G,      KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,   KC_QUOT,   KC_ENT,    KC_ENT,
    KC_LSHIFT, KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,      KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,   KC_RSFT,                         KC_UP,
    KC_LCTRL,  KC_LALT,   KC_LGUI,   KC_SPC,               KC_SPC,               KC_SPC,               KC_RGUI,   _KC_NUM,   _KC_FN,               KC_LEFT,   KC_DOWN,   KC_RIGHT
  ),

  [_L_NUM] = LAYOUT(
    KC_GRV,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,      KC_6,      KC_7,      KC_8,      KC_9,      KC_0,      KC_MINS,   KC_EQL,    _______,   _______,   _______,
    _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,
    _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,                         _______,
    _______,   _______,   _______,   _______,              _______,              _______,              _______,   _______,   _______,              _______,   _______,   _______
  ),

  [_L_FN] = LAYOUT(
    KC_ESC,    KC_F1,     KC_F2,     KC_F3,     KC_F4,     KC_F5,     KC_F6,     KC_F7,     KC_F8,     KC_F9,     KC_F10,    KC_F11,    KC_F12,    _______,   KC_DEL,    _______,
    _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,
    _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,   _______,                         KC_PGUP,
    _______,   _______,   _______,   _______,              _______,              _______,              _______,   _______,   _______,              KC_HOME,   KC_PGDN,   KC_END
  )
};
