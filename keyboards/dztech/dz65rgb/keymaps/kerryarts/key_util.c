#include "key_util.h"

#include "keycode.h"
#include "quantum_keycodes.h"
#include <stdint.h>

// Maps LED index to the color of the key cap in my key cap set
static const uint8_t key_cap_color_map[DRIVER_1_LED_TOTAL] = {
    KC_ORANGE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE , KC_GRAY  , KC_GRAY,
    KC_GRAY  , KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE , KC_GRAY  , KC_GRAY,
    KC_GRAY  , KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE,            KC_GRAY  , KC_GRAY,
    KC_GRAY  , KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE,           KC_GRAY  , KC_ORANGE, KC_GRAY,
    KC_GRAY  , KC_GRAY , KC_GRAY ,                     KC_WHITE,                               KC_GRAY , KC_GRAY , KC_GRAY , KC_ORANGE, KC_ORANGE, KC_ORANGE
};

enum key_cap_color get_key_cap_color(uint8_t led_index) {
    return key_cap_color_map[led_index];
}

// Returns true if they can be modified by the shift or CAPS LOCK key
bool is_key_code_shiftable(uint16_t key_code) {
    return (key_code >= KC_A && key_code <= KC_0) // A-Z, 1-0
        || key_code == KC_MINUS
        || key_code == KC_EQUAL
        || key_code == KC_LBRACKET
        || key_code == KC_RBRACKET
        || key_code == KC_BSLASH
        || key_code == KC_SCOLON
        || key_code == KC_QUOTE
        || key_code == KC_GRAVE // AKA backtick `
        || key_code == KC_COMMA
        || key_code == KC_DOT
        || key_code == KC_SLASH;
}

bool is_key_code_macro(uint16_t key_code) {
    return key_code >= DYN_REC_START1 && key_code <= DYN_MACRO_PLAY2;
}
