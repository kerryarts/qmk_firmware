#include QMK_KEYBOARD_H
#include "layers.c"
#include "print.h"
#include "color.h"
#include "keycode.h"
#include "keyboard.h"
#include "host.h"
#include "action.h"
#include "action_layer.h"
#include "quantum_keycodes.h"
#include "rgb_matrix.h"
// #include "rgb_matrix_types.h"

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
#define HUE_INVALID 1 // Represents no value from the hue picker. Safe, because its mathmetically it can't be generated.
#define RGB_MODE_INVALID 255 // Represents no selection for the RGB mode
#define BYTE_INVALID 1 // Represents no selection from the byte picket. Safe, because its mathmetically it can't be generated.
#define HUE_INC 9 // Roughly matches 12°, same division as the hue picker
#define HUE_PICKER_OFFSET HUE_INVALID // 0 to have the current hue at the left of the picer, 128 to centre. HUE_INVALID to disable.
#define LED_INDEX_CAPS_LOCK 30

/*** TYPE DEF ***/

// TODO: rename, key codes also have the KC prefix
enum key_cap_color {
    KC_ORANGE = 1,
    KC_GRAY,
    KC_WHITE
};

enum rgb_layer_mode {
    RLM_PREVIEW = 1,
    RLM_MODE,
    RLM_SPEED,
    RLM_HUE,
    RLM_SAT,
    RLM_VAL
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

/*** FIELDS ***/
static enum rgb_layer_mode _rgb_layer_mode = RLM_PREVIEW;
static bool _rgb_layer_mode_visible = false;
static bool _rgb_layer_mode_changed = false;

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

// Returns the hue value at the given position, from 0-255
// Internally scales based on increments of 12 degrees per key, then maps the range of 0-360 deg to 0-256
// TODO: This is just dividing up the 256 range but with extra steps
uint8_t get_hue_from_key_pos(uint8_t curr_hue, keypos_t key_pos) {
    uint8_t three_deg;
    // W -> P
    if (key_pos.row == 1 && key_pos.col >= 2 && key_pos.col <= 11) {
        three_deg = 8 + ((key_pos.col - 2) * 12);
    }
    // A -> ;
    else if (key_pos.row == 2 && key_pos.col >= 1 && key_pos.col <= 10) {
        three_deg = 0 + ((key_pos.col - 1) * 12);
    }
    // Z -> .
    else if (key_pos.row == 3 && key_pos.col >= 1 && key_pos.col <= 10) {
        three_deg = 4 + ((key_pos.col - 1) * 12);
    }
    else {
        return HUE_INVALID;
    }

    // Same as (deg/360) * 256, but avoids doing FP
    uint8_t new_hue = ((three_deg % 120) * 256) / 120;

    if (HUE_PICKER_OFFSET == HUE_INVALID) {
        return new_hue;
    }

    // Effectively sets the position of where curr_hue sits on the hue picker
    return curr_hue + HUE_PICKER_OFFSET + new_hue;
}

uint8_t get_byte_from_key_pos(keypos_t key_pos) {
    // Keys [1] to [+]
    // [+] ends up being 255.2, which gets truncated down to 255, the max byte value
    if (key_pos.row == 0 && key_pos.col >= 1 && key_pos.col <= 12) {
        // Should actually be '* 23.2', but we multiply (then divide) by 10 to avoid doing fp
        return ((key_pos.col - 1) * 232) / 10;
    }

    return BYTE_INVALID;
}

uint8_t get_rgb_mode_from_key_code(uint16_t key_code) {
    switch (key_code) {
        case KC_P: // [P]lain
            return RGB_MATRIX_SOLID_COLOR;
        case KC_G: // [G]radient
            return RGB_MATRIX_GRADIENT_UP_DOWN;
        // case KC_:
        //     return RGB_MATRIX_GRADIENT_LEFT_RIGHT;
        case KC_T: // [T]op to bottom
            return RGB_MATRIX_CYCLE_UP_DOWN;
        case KC_L: // [L]eft to right
            return RGB_MATRIX_RAINBOW_MOVING_CHEVRON;
        case KC_R: // [R]ainbow
            return RGB_MATRIX_CYCLE_OUT_IN;
        // case KC_:
        //     return RGB_MATRIX_CYCLE_OUT_IN_DUAL;
        case KC_U: // [U]mbrella
            return RGB_MATRIX_CYCLE_SPIRAL;
        case KC_M: // [M]ix
            return RGB_MATRIX_DUAL_BEACON;
        case KC_B: // [B]reating
            return RGB_MATRIX_HUE_BREATHING;
        case KC_H: // [H]eatmap
            return RGB_MATRIX_TYPING_HEATMAP;
        case KC_S: // [S]pot
            return RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE;
        // case KC_:
        //     return RGB_MATRIX_SOLID_REACTIVE_MULTICROSS;
        case KC_C: // [C]ross
            return RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS;
        // case KC_:
        //     return RGB_MATRIX_MULTISPLASH;
        case KC_W: // [W]ave
            return RGB_MATRIX_SOLID_MULTISPLASH;
        default:
            return RGB_MODE_INVALID;
    }
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

    // For CAPS LOCK or shift. Hue is SHIFTed forward.
    HSV shift_hsv = { .h = (curr_hsv.h + HUE_INC * 2) % 256, .s = curr_hsv.s, .v = curr_hsv.v };
    RGB shift_rgb = hsv_to_rgb(shift_hsv);

    // For keys on the function layer. Hue is shifted back.
    HSV func_hsv = { .h = (curr_hsv.h + 256 - (HUE_INC * 2)) % 256, .s = curr_hsv.s, .v = curr_hsv.v };
    RGB func_rgb = hsv_to_rgb(func_hsv);

    // For keys which control switching layers. Hue is opposite on the color wheel, for maximum contrast.
    HSV layer_hsv = { .h = (curr_hsv.h + 128) % 256, .s = curr_hsv.s, .v = curr_hsv.v };
    RGB layer_rgb = hsv_to_rgb(layer_hsv);

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; key_row++) {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; key_col++) {
            uint8_t led_index = g_led_config.matrix_co[key_row][key_col];

            // Early exit if there is no LED (or key, really) at this col+row position
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

            // Always light the CAPS LOCK key when CAPS LOCK is on
            if (caps_lock_on && led_index == LED_INDEX_CAPS_LOCK) {
                rgb_matrix_set_color(led_index, shift_rgb.r, shift_rgb.g, shift_rgb.b);
                continue;
            }

            // If CAPS LOCK is turned on or shift is being held in an appropriate rgb mode, highlight the white 'shiftable' keys
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
                case CL_RGB: ; // empty statement to satisfy compiler
                    uint8_t new_hue = curr_hsv.h;
                    uint8_t new_sat = curr_hsv.s;
                    uint8_t new_val = curr_hsv.v;
                    enum led_change { LC_OFF = 0, LC_NEW = 1, LC_NONE = 2 } led_change;
                    enum rgb_layer_mode visible_rgb_layer_mode = _rgb_layer_mode_visible ? _rgb_layer_mode : RLM_PREVIEW;

                    switch (visible_rgb_layer_mode) {
                        case RLM_PREVIEW:
                            switch (key_cap_color) {
                                case KC_ORANGE:
                                    new_hue = 21;
                                    new_sat = 255;
                                    new_val = 255;
                                    led_change = LC_NEW;
                                    break;
                                case KC_GRAY:
                                    led_change = LC_OFF;
                                    break;
                                case KC_WHITE:
                                    // Let the RGB mode be used on all keys, so we can preview it
                                    led_change = LC_NONE;
                                    break;
                            }
                            break;
                        case RLM_MODE: ;
                            uint16_t key_code_on_base = keymap_key_to_keycode(CL_BASE, key_pos);
                            uint8_t rgb_mode = get_rgb_mode_from_key_code(key_code_on_base);

                            led_change = rgb_mode != RGB_MODE_INVALID;
                            break;
                        case RLM_HUE: ;
                            new_hue = get_hue_from_key_pos(curr_hsv.h, key_pos);
                            led_change = new_hue != HUE_INVALID;
                            break;
                        case RLM_SPEED: ;
                            // Alter the lightness value for speed
                            new_val = get_byte_from_key_pos(key_pos);
                            new_sat = 255;
                            led_change = new_val != BYTE_INVALID;
                            break;
                        case RLM_SAT: ;
                            new_sat = get_byte_from_key_pos(key_pos);
                            led_change = new_sat != BYTE_INVALID;
                            break;
                        case RLM_VAL: ;
                            new_val = get_byte_from_key_pos(key_pos);
                            led_change = new_val != BYTE_INVALID;
                            break;
                    }

                    switch (led_change) {
                        case LC_OFF: ;
                            rgb_matrix_set_color(led_index, RGB_OFF);
                            break;
                        case LC_NEW: ;
                            HSV new_hsv = { .h = new_hue, .s = new_sat, .v = new_val };
                            RGB new_rgb = hsv_to_rgb(new_hsv);
                            rgb_matrix_set_color(led_index, new_rgb.r, new_rgb.g, new_rgb.b);
                            break;
                        case LC_NONE:
                            // Don't set at this index, so the RGB mode effect is used instead
                            break;
                    }
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
    // The key code this key press is for on the BASE layer. Its easier to recognise which key is pressed using this.
    uint16_t key_code_on_base = keymap_key_to_keycode(CL_BASE, record->event.key);

    #ifdef CONSOLE_ENABLE
    if (record->event.pressed) {
        dprintf("KL: kc: 0x%04X, bkc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n",
            keycode, key_code_on_base, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
    }
    #endif

    switch (get_highest_layer(layer_state)) {
        case CL_BASE:
            break;
        case CL_FUNC:
            break;
        case CL_RGB: ;
            enum rgb_layer_mode new_rgb_layer_mode = 0;
            switch (keycode) {
                case CKC_RLM_MODE:
                    new_rgb_layer_mode = RLM_MODE;
                    break;
                case CKC_RLM_SPEED:
                    new_rgb_layer_mode = RLM_SPEED;
                    break;
                case CKC_RLM_HUE:
                    new_rgb_layer_mode = RLM_HUE;
                    break;
                case CKC_RLM_SAT:
                    new_rgb_layer_mode = RLM_SAT;
                    break;
                case CKC_RLM_VAL:
                    new_rgb_layer_mode = RLM_VAL;
                    break;
            }

            if (new_rgb_layer_mode > 0) {
                // Key Down
                if (record->event.pressed) {
                    // If already in this mode, then toggle off
                    if (new_rgb_layer_mode == _rgb_layer_mode) {
                        _rgb_layer_mode = RLM_PREVIEW;
                    }
                    // Else switch to new mode
                    else {
                        _rgb_layer_mode = new_rgb_layer_mode;
                        _rgb_layer_mode_visible = false;
                        _rgb_layer_mode_changed = false;
                    }
                }
                // Key Up
                else {
                    // If a change was made while the key was held, then we are done, so switch back
                    // This makes the mode toggling act like a momentary layer switch
                    if (_rgb_layer_mode_changed) {
                        _rgb_layer_mode = RLM_PREVIEW;
                    }
                    // Else toggle the mode specific lighting on if this is still our layer
                    // This makes the mode toggling act like a toggle layer switch
                    // TODO: Could probably actually just use the built-in layer switching for this, but it would require a new layer for each mode zzzzz
                    else if (new_rgb_layer_mode == _rgb_layer_mode) {
                        _rgb_layer_mode_visible = true;
                    }
                }

                return false;
            }

            // On key down, make the change
            if (record->event.pressed) {
                HSV curr_hsv = rgb_matrix_get_hsv();

                switch (_rgb_layer_mode) {
                    case RLM_PREVIEW:
                        break;
                    case RLM_MODE: ;
                        uint8_t rgb_mode = get_rgb_mode_from_key_code(key_code_on_base);

                        if (rgb_mode != RGB_MODE_INVALID) {
                            rgb_matrix_mode_noeeprom(rgb_mode);
                            _rgb_layer_mode_changed = true;

                            if (_rgb_layer_mode_visible) {
                                _rgb_layer_mode = RLM_PREVIEW;
                            }
                            return false;
                        }
                        break;
                    case RLM_SPEED: ;
                        uint8_t new_speed = get_byte_from_key_pos(record->event.key);

                        if (new_speed != BYTE_INVALID) {
                            rgb_matrix_set_speed_noeeprom(new_speed);
                            _rgb_layer_mode_changed = true;

                            if (_rgb_layer_mode_visible) {
                                _rgb_layer_mode = RLM_PREVIEW;
                            }
                            return false;
                        }
                        break;
                    case RLM_HUE: ;
                        uint8_t new_hue = get_hue_from_key_pos(curr_hsv.h, record->event.key);

                        if (new_hue != HUE_INVALID) {
                            rgb_matrix_sethsv_noeeprom(new_hue, curr_hsv.s, curr_hsv.v);
                            _rgb_layer_mode_changed = true;

                            if (_rgb_layer_mode_visible) {
                                _rgb_layer_mode = RLM_PREVIEW;
                            }
                            return false;
                        }
                        break;
                    case RLM_SAT: ;
                        uint8_t new_sat = get_byte_from_key_pos(record->event.key);

                        if (new_sat != BYTE_INVALID) {
                            rgb_matrix_sethsv_noeeprom(curr_hsv.h, new_sat, curr_hsv.v);
                            _rgb_layer_mode_changed = true;

                            if (_rgb_layer_mode_visible) {
                                _rgb_layer_mode = RLM_PREVIEW;
                            }
                            return false;
                        }
                        break;
                    case RLM_VAL: ;
                        uint8_t new_val = get_byte_from_key_pos(record->event.key);

                        if (new_val != BYTE_INVALID) {
                            rgb_matrix_sethsv_noeeprom(curr_hsv.h, curr_hsv.s, new_val);
                            _rgb_layer_mode_changed = true;

                            if (_rgb_layer_mode_visible) {
                                _rgb_layer_mode = RLM_PREVIEW;
                            }
                            return false;
                        }
                        break;
                }
            }

            #ifdef CONSOLE_ENABLE
            dprintf("RGB Layer. ADJ MODE: %u\n", _rgb_layer_mode);
            #endif

            break;
        case CL_SYS:
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

            // Always start in preview mode
            _rgb_layer_mode = RLM_PREVIEW;
            break;
        case CL_SYS:
            break;
        default:
            break;
    }
    return state;
}
