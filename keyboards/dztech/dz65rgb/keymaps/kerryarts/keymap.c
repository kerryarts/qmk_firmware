#include QMK_KEYBOARD_H
#include "layers.c"
#include "print.h"

// TODO: Use these instead of straight RGB_RED and RGB_ORANGE
// HSV (0-255 scaled)        H,   S,   V    Hue°  Hex
// #define HSV_RED           0, 255, 255 //   0   #FF0000 Defined in color.h
#define HSV_RED_OR          11, 255, 255 //  15   #FF4000 Red-orange mix
#define HSV_INTER_OR        13, 255, 255 //  19   #FF4F00 International orange, the best orange
#define HSV_INTER2_OR       14, 255, 255 //  19   #FF4F00 International orange, rounded to a multiple of 7
#define HSV_SAFETY_OR       20, 255, 255 //  28   #FF7900 Safety orange
#define HSV_FULL_OR         21, 255, 255 //  30   #FF8000 Full orange
// #define HSV_ORANGE       28, 255, 255 //  40   #FFAA00 Defined in color.h

#define KEY_COUNT 68
#define ADJ_NO_VALUE 1 // Represents no value from the adjustment layer. Safe, because its mathmetically it can't be generated.
#define HUE_INC 9 // Roughly matches 12°, same division as the hue picker

/*** TYPE DEF ***/

// Copy to layers.c after auto generation
enum custom_layers {
    CL_BASE,
    CL_FUNC,
    CL_RGB,
    CL_SYS,
    CL_ADJ
};

enum key_cap_color {
    KC_ORANGE,
    KC_GRAY,
    KC_WHITE
};

/*** CONSTS ***/

const HSV HSV_NONE = { .h = 0, .s = 0, .v = 0};

// Maps LED index to the color of the key cap in my key cap set
const uint8_t key_cap_color_map[KEY_COUNT] = {
    KC_ORANGE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE , KC_GRAY  , KC_GRAY,
    KC_GRAY  , KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE , KC_GRAY  , KC_GRAY,
    KC_GRAY  , KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE,            KC_GRAY  , KC_GRAY,
    KC_GRAY  , KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE, KC_WHITE,           KC_GRAY  , KC_ORANGE, KC_GRAY,
    KC_GRAY  , KC_GRAY , KC_GRAY ,                     KC_WHITE,                               KC_GRAY , KC_GRAY , KC_GRAY , KC_ORANGE, KC_ORANGE, KC_ORANGE
};

/*** USER FUNC ***/

// Returns true if they can be modified by the shift or CAPS LOCK key
bool key_code_is_shiftable(uint16_t key_code) {
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

/*** KEYBOARD FUNCS ***/

// Returns the adjustment value at the given position, from 0-255
// Internally scales based on increments of 12 degrees per key, then maps the range of 0-360 deg to 0-256
// TODO: This is just dividing up the 256 range but with extra steps
uint8_t get_adj_value_from_key_pos(keypos_t key_pos) {
    uint8_t three_deg;
    // W -> P
    if (key_pos.row == 1 && key_pos.col >= 2 && key_pos.col <= 11) {
        three_deg = 8 + ((key_pos.col - 2) * 12);
    }
    // A -> ;
    // The range stops at L, so both A and ; will both return zero
    else if (key_pos.row == 2 && key_pos.col >= 1 && key_pos.col <= 11) {
        three_deg = 0 + ((key_pos.col - 1) * 12);
    }
    // Z -> .
    else if (key_pos.row == 3 && key_pos.col >= 1 && key_pos.col <= 10) {
        three_deg = 4 + ((key_pos.col - 1) * 12);
    }
    else {
        return ADJ_NO_VALUE;
    }

    // Same as (deg/360) * 256, but avoids doing FP
    return ((three_deg % 120) * 256) / 120;
}

uint8_t get_hue_from_adj_val(uint8_t curr_hue, uint8_t adj_val) {
    // Offset the hue by the current hue + 50%
    // This makes the current hue centered in the middle of the keyboard at the H key
    return (curr_hue + 128 + adj_val) % 256;
}

void rgb_matrix_indicators_user(void) {
    bool caps_lock_on = host_keyboard_led_state().caps_lock;
    bool shift_key_held = get_mods() & MOD_MASK_SHIFT;

    uint8_t rgb_mode = rgb_matrix_get_mode();
    bool rgb_mode_is_single_hue =
        rgb_mode == RGB_MATRIX_SOLID_COLOR ||
        rgb_mode == RGB_MATRIX_GRADIENT_UP_DOWN ||
        rgb_mode == RGB_MATRIX_GRADIENT_LEFT_RIGHT ||
        rgb_mode == RGB_MATRIX_HUE_BREATHING;

    uint8_t layer_index = get_highest_layer(layer_state); // Highest layer, but there might be others enabled

    HSV curr_hsv = rgb_matrix_get_hsv();
    RGB curr_rgb = hsv_to_rgb(curr_hsv);

    HSV shift_hsv = { .h = (curr_hsv.h + HUE_INC * 2) % 256, .s = curr_hsv.s, .v = curr_hsv.v };
    RGB shift_rgb = hsv_to_rgb(shift_hsv);

    HSV func_hsv = { .h = (curr_hsv.h + 256 - (HUE_INC * 2)) % 256, .s = curr_hsv.s, .v = curr_hsv.v };
    RGB func_rgb = hsv_to_rgb(func_hsv);

    HSV layer_hsv = { .h = (curr_hsv.h + 128) % 256, .s = curr_hsv.s, .v = curr_hsv.v };
    RGB layer_rgb = hsv_to_rgb(layer_hsv);

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; key_row++) {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; key_col++) {
            uint8_t led_index = g_led_config.matrix_co[key_row][key_col];

            // Early exit if there is no LED at this col+row position
            if (led_index == NO_LED) {
                continue;
            }

            enum key_cap_color key_cap_color = key_cap_color_map[led_index];
            keypos_t key_pos = { .row = key_row, .col = key_col };
            uint16_t key_code = keymap_key_to_keycode(layer_index, key_pos);
            bool key_code_is_mapped = key_code >= KC_A; // Excludes KC_NO, KC_ROLL_OVER, KC_POST_FAIL, KC_UNDEFINED
            bool key_code_is_layer = key_code >= QK_LAYER_TAP && key_code <= QK_LAYER_TAP_TOGGLE_MAX; // TODO: The comment in quantum_keycodes.h said not to use these directly...shhh don't tell anyone
            bool key_code_is_media = (key_code >= KC_AUDIO_MUTE && key_code <= KC_MEDIA_EJECT) || key_code == KC_MEDIA_FAST_FORWARD || key_code == KC_MEDIA_REWIND;

            // On anything but the base layer, highlight layer switch keys
            if (layer_index != CL_BASE && key_code_is_layer) {
                rgb_matrix_set_color(led_index, layer_rgb.r, layer_rgb.g, layer_rgb.b);
                continue;
            }

            // If caps lock is turned on or shift is being held, highlight the white 'shiftable' keys
            if ((caps_lock_on || (shift_key_held && rgb_mode_is_single_hue))
                && key_cap_color == KC_WHITE
                && (key_code_is_shiftable(key_code) || (layer_index == CL_FUNC && key_code_is_mapped))) {
                // Shift the hue backward a bit
                rgb_matrix_set_color(led_index, shift_rgb.r, shift_rgb.g, shift_rgb.b);
                continue;
            }

            // Else light the mapped key based on the current layer
            switch (layer_index) {
                case CL_BASE:
                    // Don't light up the spacebar, since it has poor light distribution
                    if (!key_code_is_mapped || key_code == KC_SPACE) {
                        rgb_matrix_set_color(led_index, RGB_OFF);
                    }
                    else {
                        switch (key_cap_color) {
                            // Make the orange keys even more orange
                            case KC_ORANGE:
                                rgb_matrix_set_color(led_index, RGB_ORANGE);
                                break;
                            // Gray keys have poor light distribution, keep them off
                            case KC_GRAY:
                                rgb_matrix_set_color(led_index, RGB_OFF);
                                break;
                            case KC_WHITE:
                                // Let the RGB mode be used
                                break;
                        }
                    }
                    break;
                case CL_FUNC:
                    if (!key_code_is_mapped) {
                        rgb_matrix_set_color(led_index, RGB_OFF);
                    }
                    else if (key_code_is_media) {
                        rgb_matrix_set_color(led_index, func_rgb.r, func_rgb.g, func_rgb.b);
                    }
                    else {
                        rgb_matrix_set_color(led_index, curr_rgb.r, curr_rgb.g, curr_rgb.b);
                    }
                    break;
                case CL_SYS:
                    if (!key_code_is_mapped) {
                        rgb_matrix_set_color(led_index, RGB_OFF);
                    }
                    else {
                        rgb_matrix_set_color(led_index, func_rgb.r, func_rgb.g, func_rgb.b);
                    }
                    break;
                case CL_RGB:
                    switch (key_cap_color) {
                        case KC_ORANGE:
                            rgb_matrix_set_color(led_index, RGB_ORANGE);
                            break;
                        case KC_GRAY:
                            rgb_matrix_set_color(led_index, RGB_OFF);
                            break;
                        case KC_WHITE:
                            // Let the RGB mode be used on all keys, so we can preview it
                            break;
                    }
                    break;
                case CL_ADJ: ; // empty statement to satisfy compiler
                    uint8_t adj_val = get_adj_value_from_key_pos(key_pos);

                    if (adj_val != ADJ_NO_VALUE) {
                        HSV new_hsv = { .h = get_hue_from_adj_val(curr_hsv.h, adj_val), .s = curr_hsv.s, .v = curr_hsv.v };
                        RGB new_rgb = hsv_to_rgb(new_hsv);
                        rgb_matrix_set_color(led_index, new_rgb.r, new_rgb.g, new_rgb.b);
                    }
                    else {
                        rgb_matrix_set_color(led_index, RGB_OFF);
                    }
                    break;
                default:
                    rgb_matrix_set_color(led_index, RGB_OFF);
                    break;
            }
        }
    }
}

void keyboard_post_init_user(void) {
}

void matrix_init_user(void) {
}

void matrix_scan_user(void) {
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    #ifdef CONSOLE_ENABLE
    uprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
    #endif

    switch (get_highest_layer(layer_state)) {
        case CL_BASE:
            break;
        case CL_FUNC:
            break;
        case CL_RGB:
            break;
        case CL_SYS:
            break;
        case CL_ADJ: ; // empty statement to satisfy compiler
            keypos_t key_pos = { .row = record->event.key.row, .col = record->event.key.col };
            uint8_t adj_val = get_adj_value_from_key_pos(key_pos);

            if (adj_val != ADJ_NO_VALUE) {
                HSV curr_hsv = rgb_matrix_get_hsv();
                rgb_matrix_sethsv(get_hue_from_adj_val(curr_hsv.h, adj_val), curr_hsv.s, curr_hsv.v);

                // We are done changing the color, turn off this layer and go back to CL_RGB
                layer_off(CL_ADJ);
            }
            break;
        default:
            break;
    }

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case CL_BASE:
            break;
        case CL_FUNC:
            break;
        case CL_RGB:
            // Since we are editing the RGB lighting, ensure its turned on
            rgb_matrix_enable_noeeprom();
            break;
        case CL_SYS:
            break;
        case CL_ADJ:
            rgb_matrix_enable_noeeprom();
            break;
        default:
            break;
    }
    return state;
}
