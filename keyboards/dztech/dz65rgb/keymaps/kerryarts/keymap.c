#include QMK_KEYBOARD_H

#include "key_util.h"
#include "custom_layers.h"
#include "tap_dance.h"
#include "rgb_layer.h"
#include "rgb_util.h"
#include "dynamic_macro.h"
#include "wpm_vu.h"

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
    enum rgb_mode_flags rgb_mode_flags = get_rgb_mode_flags(rgb_mode);
    uint8_t layer_index = get_highest_layer(layer_state); // Highest layer, but there might be others enabled

    HSV curr_hsv = rgb_matrix_get_hsv();
    HSV shift_hsv = inc_hsv(curr_hsv);
    HSV func_hsv = dec_hsv(curr_hsv);
    HSV layer_hsv = inv_hsv(curr_hsv);

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; key_row++) {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; key_col++) {
            uint8_t led_index = g_led_config.matrix_co[key_row][key_col];

            // Early exit if there is no LED (or key, really) at this col+row position
            if (led_index == NO_LED) {
                continue;
            }

            enum key_cap_color key_cap_color = get_key_cap_color(led_index);
            keypos_t key_pos = { .row = key_row, .col = key_col };
            uint16_t key_code = keymap_key_to_keycode(layer_index, key_pos);
            HSV new_hsv = curr_hsv;

            if (process_led_macro(led_index, key_pos, key_code, &new_hsv)) {
                // new_hsv set via pointer
            }
            // On anything but the base layer, highlight layer switch keys
            else if (layer_index != CL_BASE && is_key_code_layer(key_code)) {
                new_hsv = layer_hsv;
            }
            // Always light the CAPS LOCK key when CAPS LOCK is on
            else if (caps_lock_on && led_index == LED_INDEX_CAPS_LOCK) {
                // Keep same HSV
            }
            // If CAPS LOCK is turned on or shift is being held in an appropriate rgb mode, highlight the white 'shiftable' keys
            else if ((caps_lock_on || shift_key_held)
                && (rgb_mode_flags & RMF_HUE_SINGLE) > 0
                && (rgb_mode_flags & (RMF_STYLE_STAIC | RMF_STYLE_ANIM)) > 0
                && key_cap_color == KC_WHITE
                && is_key_code_shiftable(key_code)) {
                new_hsv = shift_hsv;
            }
            else if (key_cap_color == KC_ORANGE) {
                new_hsv = HSV_RED_ORANGE;
            }
            else if (layer_index == CL_RGB) {
                if (!process_led_rgb_layer(led_index, key_pos, key_code, &new_hsv)) {
                    continue;
                }
            }
            else if (!is_key_code_mapped(key_code)) {
                new_hsv = HSV_NONE;
            }
            // On the base layer, don't light up the gray keys or space (because they has poor light distribution)
            else if (layer_index == CL_BASE && (key_cap_color == KC_GRAY || key_code == KC_SPACE)) {
                new_hsv = HSV_NONE;
            }
            else if (is_key_code_func(key_code)) {
                new_hsv = func_hsv;
            }
            // If not on the base layer (ie: the func layer), and a reactive RGB mode is enabled (so LEDs are only on when pressed), highlight the mapped keys so we can see them
            else if (layer_index != CL_BASE && (rgb_mode_flags & RMF_STLYE_REACTIVE) > 0) {
                // Keep same HSV
            }
            else if (process_led_wpm(led_index, key_pos, key_code, &new_hsv)) {
                // new_hsv set via pointer
            }
            else {
                // Don't set a color, let the RGB mode apply
                continue;
            }

            RGB new_rgb = hsv_to_rgb(new_hsv);
            rgb_matrix_set_color(led_index, new_rgb.r, new_rgb.g, new_rgb.b);
        }
    }
}

void keyboard_post_init_user(void) {
    keyboard_post_init_rgb_util();
    keyboard_post_init_wpm();
}

void matrix_init_user(void) {
}

void matrix_scan_user(void) {
    matrix_scan_wpm();
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
    bool continue_processing =
        process_record_macro(keycode, record) &&
        process_record_wpm(keycode, record);

    if (!continue_processing) {
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

bool led_update_user(led_t led_state) {
    return true;
}
