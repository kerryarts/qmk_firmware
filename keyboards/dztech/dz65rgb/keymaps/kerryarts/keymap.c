#include QMK_KEYBOARD_H

#include "key_util.h"
#include "custom_layers.h"
#include "tap_dance.h"
#include "rgb_layer.h"
#include "rgb_util.h"
#include "dynamic_macro.h"

#include "print.h"
#include "color.h"
#include "keycode.h"
#include "keyboard.h"
#include "host.h"
#include "action.h"
#include "action_layer.h"
#include "quantum_keycodes.h"
#include "rgb_matrix.h"



/*** KEYBOARD HOOKS - USER ***/

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

            HSV new_hsv = curr_hsv;

            enum key_cap_color key_cap_color = get_key_cap_color(led_index);
            keypos_t key_pos = { .row = key_row, .col = key_col };
            uint16_t key_code = keymap_key_to_keycode(layer_index, key_pos);
            bool key_code_is_mapped = key_code >= KC_A; // Excludes KC_NO, KC_ROLL_OVER, KC_POST_FAIL, KC_UNDEFINED
            bool key_code_is_layer = key_code >= QK_LAYER_TAP && key_code <= QK_LAYER_TAP_TOGGLE_MAX; // TODO: The comment in quantum_keycodes.h said not to use these directly...shhh don't tell anyone
            bool key_code_is_standard = key_code >= KC_A && key_code <= KC_RGUI; // Keys on a standard keyboard

            if (process_led_macro(led_index, key_pos, key_code, &new_hsv)) {
                RGB new_rgb = hsv_to_rgb(new_hsv);
                rgb_matrix_set_color(led_index, new_rgb.r, new_rgb.g, new_rgb.b);
                continue;
            }

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
                && (is_key_code_shiftable(key_code) || (layer_index == CL_FUNC && key_code_is_mapped))) {
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
                    else if (key_code_is_standard) {
                        rgb_matrix_set_color(led_index, curr_rgb.r, curr_rgb.g, curr_rgb.b);
                    }
                    else {
                        rgb_matrix_set_color(led_index, func_rgb.r, func_rgb.g, func_rgb.b);
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
                case CL_RGB: ;
                    if (process_led_rgb_layer(led_index, key_pos, key_code, &new_hsv)) {
                        RGB new_rgb = hsv_to_rgb(new_hsv);
                        rgb_matrix_set_color(led_index, new_rgb.r, new_rgb.g, new_rgb.b);
                    }
                    break;
            }
        }
    }
}

void keyboard_post_init_user(void) {
    keyboard_post_init_rgb_util();
}

void matrix_init_user(void) {
}

void matrix_scan_user(void) {
}

void suspend_power_down_user(void) {
    // On PC suspend, ensure we are only on the base layer
    // layer_move(CL_BASE);
}

void suspend_wakeup_init_user(void) {
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    #ifdef CONSOLE_ENABLE
    if (record->event.pressed) {
        // The key code this key press is for on the BASE layer. Its easier to recognise which key is pressed using this.
        uint16_t key_code_on_base = keymap_key_to_keycode(CL_BASE, record->event.key);

        dprintf("KL: kc: 0x%04X, bkc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n",
            keycode, key_code_on_base, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
    }
    #endif

    // Layer indepedent key processing
    if (!process_record_macro(keycode, record)) {
        return false;
    }

    // Layer specific key processing
    switch (get_highest_layer(layer_state)) {
        case CL_BASE:
            break;
        case CL_FUNC:
            break;
        case CL_RGB:
            return process_record_rgb_layer(keycode, record);
        case CL_SYS:
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
            return layer_state_set_rgb_layer(state);
        case CL_SYS:
            break;
    }
    return state;
}
