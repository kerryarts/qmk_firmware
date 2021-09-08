#include QMK_KEYBOARD_H
#include "layers.c"
#include "print.h"

// TODO: Use these instead of straight RGB_RED and RGB_ORANGE
// HSV (0-255 scaled)        H,   S,   V    Hue°  Hex
// #define HSV_RED           0, 255, 255 //   0   #FF0000 Defined in color.h
#define HSV_RED_OR          11, 255, 255 //  15   #FF4000 Red-orange mix
#define HSV_INTER_OR        13, 255, 255 //  19   #FF4F00 International orange, the best orange
#define HSV_SAFETY_OR       20, 255, 255 //  28   #FF7900 Safety orange
#define HSV_FULL_OR         21, 255, 255 //  30   #FF8000 Full orange
// #define HSV_ORANGE       28, 255, 255 //  40   #FFAA00 Defined in color.h

#define KEY_COUNT 68

/*** TYPE DEF ***/

// Copy to layers.c after auto generation
// enum custom_layers {
//     CL_BASE,
//     CL_FUNC,
//     CL_SYS
// };

enum key_cap_color {
    KC_ORANGE,
    KC_GRAY,
    KC_WHITE
};

enum key_type {
    KT_ALPHA,
    KT_NUMERIC,
    KT_SYMBOL,
    KT_MODIFIER,
    KT_SPECIAL,
    KT_FUNCTION
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

void update_key_led(uint8_t layer_index, uint8_t key_row, uint8_t key_col, led_t led_state, bool shift_held) {
    uint8_t led_index = g_led_config.matrix_co[key_row][key_col];

    // Early exit if there is no LED at this col+row position
    if (led_index == NO_LED) {
        return;
    }

    enum key_cap_color key_cap_color = key_cap_color_map[led_index];
    uint16_t key_code = pgm_read_word(&keymaps[layer_index][key_row][key_col]);
    bool key_code_mapped = key_code >= KC_A; // Excludes KC_NO (key not mapped) and KC_TRNS (key is transparent), among others

    // If this key isnt mapped to anything, keep it off
    if (!key_code_mapped) {
        rgb_matrix_set_color(led_index, RGB_OFF);
    }
    // If caps lock is turned on or shift is being held, highlight the white 'shiftable' keys
    // TODO: caps lock doesn't currently have a mapping
    else if ((led_state.caps_lock || shift_held) && key_cap_color == KC_WHITE && key_code_is_shiftable(key_code)) {
        rgb_matrix_set_color(led_index, RGB_RED);
    }
    // Else light the mapped key based on the current layer
    else {
        switch (layer_index) {
            case CL_BASE:
                // For the base layer to look nice
                // - Only light up the light colored key caps which can easily let light through them
                // - Don't light up the spacebar, since it has poor light distribution
                // TODO: See if we can have an animated effect on the base layer, on only these keys
                if ((key_cap_color == KC_WHITE || key_cap_color == KC_ORANGE) && key_code != KC_SPACE) {
                    rgb_matrix_set_color(led_index, RGB_ORANGE);
                }
                else {
                    rgb_matrix_set_color(led_index, RGB_OFF);
                }
                break;
            case CL_FUNC:
                rgb_matrix_set_color(led_index, RGB_BLUE);
                break;
            case CL_SYS:
                rgb_matrix_set_color(led_index, RGB_GREEN);
                break;
            default:
                rgb_matrix_set_color(led_index, RGB_OFF);
                break;
        }
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

    const uint8_t modifier_mask = get_mods();
    const uint8_t shift_held = modifier_mask & (MOD_BIT(KC_LSHIFT)|MOD_BIT(KC_RSHIFT));
    // const uint8_t ctrl_held = modifier_mask & (MOD_BIT(KC_LCTL)|MOD_BIT(KC_RCTL));

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; key_row++) {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; key_col++) {
            update_key_led(layer_index, key_row, key_col, led_state, shift_held);
        }
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        default:
            break;
    }
    return state;
}
