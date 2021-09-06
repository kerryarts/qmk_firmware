#include QMK_KEYBOARD_H

/* THIS FILE WAS GENERATED!
 *
 * This file was generated by qmk json2c. You may or may not want to
 * edit it directly.
 */

enum custom_layers {
    CL_BASE,
    CL_FUNC,
    CL_SYS
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[CL_BASE] = LAYOUT_65_ansi(KC_GESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_DEL, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP, MO(1), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_PGDN, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_HOME, KC_UP, KC_END, KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_WBAK, MO(1), KC_WFWD, KC_LEFT, KC_DOWN, KC_RGHT),
	[CL_FUNC] = LAYOUT_65_ansi(KC_GRV, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_DEL, KC_PSCR, KC_TRNS, RGB_TOG, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, KC_NO, KC_NO, KC_INS, KC_NO, KC_PAUS, KC_NO, KC_NO, KC_MUTE, KC_VOLU, KC_TRNS, RGB_MOD, RGB_HUI, RGB_SAI, RGB_VAI, RGB_SPI, KC_NO, KC_NO, KC_NO, KC_SLCK, KC_NO, KC_NO, KC_MPLY, KC_VOLD, KC_TRNS, RGB_RMOD, RGB_HUD, RGB_SAD, RGB_VAD, RGB_SPD, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MRWD, KC_VOLU, KC_MFFD, KC_TRNS, KC_APP, KC_TRNS, MO(2), KC_TRNS, KC_TRNS, KC_TRNS, KC_MPRV, KC_VOLD, KC_MNXT),
	[CL_SYS] = LAYOUT_65_ansi(KC_PWR, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_SLEP, KC_NO, KC_NO, KC_NO, EEP_RST, RESET, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, DEBUG, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, NK_OFF, NK_TOGG, NK_ON, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO)
};
