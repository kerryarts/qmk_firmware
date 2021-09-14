#include QMK_KEYBOARD_H

enum custom_layers {
    CL_BASE,
    CL_FUNC,
    CL_RGB,
    CL_SYS
};

enum custom_keycodes {
    CKC_RLM_MODE = SAFE_RANGE,
    CKC_RLM_SPEED,
    CKC_RLM_HUE,
    CKC_RLM_SAT,
    CKC_RLM_VAL,

    CKC_RGB_TOG,
    CKC_RGB_SAVE,
    CKC_RGB_LOAD,
    CKC_MODE_INC,
    CKC_MODE_DEC,
    CKC_SPEED_INC,
    CKC_SPEED_DEC,
    CKC_HUE_INC,
    CKC_HUE_DEC,
    CKC_SAT_INC,
    CKC_SAT_DEC,
    CKC_VAL_INC,
    CKC_VAL_DEC
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [CL_BASE] = LAYOUT_65_ansi(
        KC_GESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_DEL,
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP,
        MO(1), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_PGDN,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_HOME, KC_UP, KC_END,
        KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_WBAK, MO(1), KC_WFWD, KC_LEFT, KC_DOWN, KC_RGHT),
    [CL_FUNC] = LAYOUT_65_ansi(
        KC_GRV, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_DEL, KC_PSCR,
        KC_CAPS, KC_P7, KC_P8, KC_P9, TO(2), TO(3), KC_NO, KC_NO, KC_INS, KC_NO, KC_PAUS, KC_NO, KC_NO, KC_MUTE, KC_VOLU,
        KC_TRNS, KC_P4, KC_P5, KC_P6, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_SLCK, KC_NO, KC_NO, KC_MPLY, KC_VOLD,
        KC_TRNS, KC_P1, KC_P2, KC_P3, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MRWD, KC_VOLU, KC_MFFD,
        KC_TRNS, KC_APP, KC_TRNS, KC_P0, KC_NO, KC_TRNS, KC_NO, KC_MPRV, KC_VOLD, KC_MNXT),
    [CL_RGB] = LAYOUT_65_ansi(
        TG(2), KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, CKC_RGB_LOAD, CKC_RGB_TOG,
        CKC_MODE_INC, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        CKC_RLM_MODE, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, CKC_RGB_SAVE, KC_NO,
        CKC_MODE_DEC, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, CKC_SAT_DEC, CKC_RLM_SAT, CKC_SAT_INC,
        CKC_SPEED_DEC, CKC_RLM_SPEED, CKC_SPEED_INC, KC_NO, CKC_VAL_DEC, CKC_RLM_VAL, CKC_VAL_INC, CKC_HUE_DEC, CKC_RLM_HUE, CKC_HUE_INC),
    [CL_SYS] = LAYOUT_65_ansi(
        TG(3), KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_PWR, KC_SLEP,
        KC_NO, KC_NO, KC_NO, EEP_RST, RESET, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, DEBUG, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, NK_OFF, NK_TOGG, NK_ON, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO),
};
