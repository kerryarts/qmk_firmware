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
#define HUE_INC 7

/*** TYPE DEF ***/

// Copy to layers.c after auto generation
// enum custom_layers {
//     CL_BASE,
//     CL_FUNC,
//     CL_RGB,
//     CL_SYS
// };

enum key_cap_color {
    KC_ORANGE,
    KC_GRAY,
    KC_WHITE
};

/*** CONSTS ***/

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

void update_key_led(uint8_t layer_index, keypos_t key_pos, led_t led_state, uint8_t modifier_mask, HSV curr_hsv, RGB curr_rgb) {
    uint8_t led_index = g_led_config.matrix_co[key_pos.row][key_pos.col];

    // Early exit if there is no LED at this col+row position
    if (led_index == NO_LED) {
        return;
    }

    enum key_cap_color key_cap_color = key_cap_color_map[led_index];
    uint16_t key_code = keymap_key_to_keycode(layer_index, key_pos);
    bool key_code_is_mapped = key_code >= KC_A; // Excludes KC_NO, KC_ROLL_OVER, KC_POST_FAIL, KC_UNDEFINED
    bool key_code_is_layer = key_code >= QK_LAYER_TAP && key_code <= QK_LAYER_TAP_TOGGLE_MAX; // TODO: The comment in quantum_keycodes.h said not to use these directly...shhh don't tell anyone
    bool key_code_is_media = (key_code >= KC_AUDIO_MUTE && key_code <= KC_MEDIA_EJECT) || key_code == KC_MEDIA_FAST_FORWARD || key_code == KC_MEDIA_REWIND;
    uint8_t matrix_mode = rgb_matrix_get_mode();

    // If caps lock is turned on or shift is being held, highlight the white 'shiftable' keys
    if ((led_state.caps_lock || (modifier_mask & MOD_MASK_SHIFT && (matrix_mode == RGB_MATRIX_SOLID_COLOR || matrix_mode == RGB_MATRIX_HUE_BREATHING)))
        && key_cap_color == KC_WHITE
        && (key_code_is_shiftable(key_code) || (layer_index == CL_FUNC && key_code_is_mapped))) {
        // Shift the hue backward a bitg
        curr_hsv.h = (255 + curr_hsv.h - (HUE_INC * 2)) % 255;
        RGB new_rgb = hsv_to_rgb(curr_hsv);
        rgb_matrix_set_color(led_index, new_rgb.r, new_rgb.g, new_rgb.b);
        return;
    }

    // On anything but the base layer, highlight layer switch keys red
    if (layer_index != CL_BASE && key_code_is_layer) {
        rgb_matrix_set_color(led_index, RGB_RED);
        return;
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
                        // Let the RGB effect be used
                        break;
                }
            }
            break;
        case CL_FUNC:
            if (!key_code_is_mapped) {
                rgb_matrix_set_color(led_index, RGB_OFF);
            }
            else if (key_code_is_media) {
                rgb_matrix_set_color(led_index, RGB_GREEN);
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
                rgb_matrix_set_color(led_index, RGB_GREEN);
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
                    // Let the RGB effect be used on all keys, so we can preview it
                    break;
            }
            break;
        default:
            rgb_matrix_set_color(led_index, RGB_OFF);
            break;
    }
}

/*** KEYBOARD FUNCS ***/

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

    return true;
}

void rgb_matrix_indicators_user(void) {
    led_t led_state = host_keyboard_led_state(); // Has .caps_lock, .num_lock, .scroll_lock
    uint8_t layer_index = get_highest_layer(layer_state); // Highest layer, but there might be others enabled
    uint8_t modifier_mask = get_mods();
    HSV curr_hsv = rgb_matrix_get_hsv();
    RGB curr_rgb = hsv_to_rgb(curr_hsv);

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; key_row++) {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; key_col++) {
            keypos_t key_pos = { .row = key_row, .col = key_col };
            update_key_led(layer_index, key_pos, led_state, modifier_mask, curr_hsv, curr_rgb);
        }
    }
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
        default:
            break;
    }
    return state;
}
