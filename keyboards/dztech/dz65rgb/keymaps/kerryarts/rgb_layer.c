#include "rgb_layer.h"

#include "rgb_util.h"
#include "custom_layers.h"
#include "key_util.h"

#include "color.h"
#include "rgb_matrix.h"
#include <stdbool.h>
#include <stdint.h>

static enum rgb_layer_mode _rgb_layer_mode = RLM_PREVIEW;
static bool _rgb_layer_mode_visible = false;
static bool _rgb_layer_mode_changed = false;

static const uint8_t _key_num_to_edit_hue[EDIT_HUE_KEY_COUNT] = {
    0, 8, 17, 25, 34, 42, 51, 59, 68, 76,
    85, 93, 102, 110, 119, 128, 136, 145, 153, 162,
    170, 179, 187, 196, 204, 213, 221, 230, 238, 247
};

static const uint8_t _key_num_to_edit_byte[EDIT_BYTE_KEY_COUNT] = {
    0, 23, 46, 69, 92, 115, 139, 162, 185, 208, 231, 255
};

/*** EDIT HUE FUNCTIONS ***/

static bool _try_get_rgb_mode_from_key_pos(keypos_t key_pos, uint8_t* rgb_mode) {
    uint16_t key_code_on_base = keymap_key_to_keycode(CL_BASE, key_pos);

    switch (key_code_on_base) {
        case KC_P: // [P]lain
            *rgb_mode = RGB_MATRIX_SOLID_COLOR;
            return true;
        case KC_G: // [G]radient
            *rgb_mode = RGB_MATRIX_GRADIENT_UP_DOWN;
            return true;
        case KC_T: // [T]op to bottom
            *rgb_mode = RGB_MATRIX_CYCLE_UP_DOWN;
            return true;
        case KC_L: // [L]eft to right
            *rgb_mode = RGB_MATRIX_RAINBOW_MOVING_CHEVRON;
            return true;
        case KC_R: // [R]ainbow
            *rgb_mode = RGB_MATRIX_CYCLE_OUT_IN;
            return true;
        case KC_U: // [U]mbrella
            *rgb_mode = RGB_MATRIX_CYCLE_SPIRAL;
            return true;
        case KC_M: // [M]ix
            *rgb_mode = RGB_MATRIX_DUAL_BEACON;
            return true;
        case KC_B: // [B]reating
            *rgb_mode = RGB_MATRIX_HUE_BREATHING;
            return true;
        case KC_H: // [H]eatmap
            *rgb_mode = RGB_MATRIX_TYPING_HEATMAP;
            return true;
        case KC_S: // [S]pot
            *rgb_mode = RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE;
            return true;
        case KC_C: // [C]ross
            *rgb_mode = RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS;
            return true;
        case KC_W: // [W]ave
            *rgb_mode = RGB_MATRIX_SOLID_MULTISPLASH;
            return true;
    }

    return false;
}

static uint8_t _get_edit_hue_from_key_num(uint8_t key_num) {
    return _key_num_to_edit_hue[key_num % EDIT_HUE_KEY_COUNT];
}

static uint8_t _get_key_num_from_edit_hue(uint8_t hue) {
    return ((hue + 1) * 10) / 85;
}

static bool _try_get_hue_from_key_pos(keypos_t key_pos, uint8_t* hue) {
    uint16_t key_num;
    // W -> [
    if (key_pos.row == 1 && key_pos.col >= 2 && key_pos.col <= 11) {
        key_num = 2 + ((key_pos.col - 2) * 3);
    }
    // A -> '
    else if (key_pos.row == 2 && key_pos.col >= 1 && key_pos.col <= 11) {
        key_num = 0 + ((key_pos.col - 1) * 3);
    }
    // Z -> /
    else if (key_pos.row == 3 && key_pos.col >= 1 && key_pos.col <= 10) {
        key_num = 1 + ((key_pos.col - 1) * 3);
    }
    else {
        return false;
    }

    *hue = _get_edit_hue_from_key_num(key_num);
    return true;
}



/*** EDIT BYTE FUNCTIONS ***/

static uint8_t _inc_edit_hue(uint8_t hue) {
    uint8_t key_num = _get_key_num_from_edit_hue(hue) + 1;
    return _get_edit_hue_from_key_num(key_num);
}

static uint8_t _dec_edit_hue(uint8_t hue) {
    uint8_t key_num = _get_key_num_from_edit_hue(hue);
    if (key_num != 0) {
        key_num--;
    }
    else {
        key_num = EDIT_HUE_KEY_COUNT - 1;
    }

    return _get_edit_hue_from_key_num(key_num);
}

static uint8_t _get_edit_byte_from_key_num(uint8_t key_num) {
    return _key_num_to_edit_byte[key_num % EDIT_BYTE_KEY_COUNT];
}

static uint8_t _get_key_num_from_edit_byte(uint8_t byte) {
    return byte / 23;
}

static uint8_t _inc_edit_byte(uint8_t byte) {
    uint8_t key_num = _get_key_num_from_edit_byte(byte) + 1;
    return _get_edit_byte_from_key_num(key_num );
}

static uint8_t _dec_edit_byte(uint8_t byte) {
    uint8_t key_num = _get_key_num_from_edit_byte(byte);
    if (key_num != 0) {
        key_num--;
    }

    return _get_edit_byte_from_key_num(key_num);
}

static bool _try_get_byte_from_key_pos(keypos_t key_pos, uint8_t* byte) {
    // Keys [1] to [+]
    if (key_pos.row == 0 && key_pos.col >= 1 && key_pos.col <= 12) {
        *byte = _get_edit_byte_from_key_num(key_pos.col - 1);
        return true;
    }

    return false;
}



/*** EDIT BYTE FUNCTIONS ***/

static bool _process_ckc_rlm(keyrecord_t* record, enum rgb_layer_mode new_rgb_layer_mode) {
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

    // Key handled, do not continue processing
    return false;
}

static void _handle_rlm_key_press(void) {
    _rgb_layer_mode_changed = true;

    if (_rgb_layer_mode_visible) {
        _rgb_layer_mode = RLM_PREVIEW;
    }
}



/*** KEYBOARD HOOKS ***/

bool process_led_rgb_layer(uint8_t led_index, keypos_t key_pos, uint16_t key_code, HSV* hsv) {
    HSV orig_hsv = *hsv;

    enum rgb_layer_mode visible_rgb_layer_mode = _rgb_layer_mode_visible ? _rgb_layer_mode : RLM_PREVIEW;
    bool key_is_current_val = false;
    bool led_on = false;

    switch (visible_rgb_layer_mode) {
        case RLM_PREVIEW: ;
            switch (get_key_cap_color(led_index)) {
                case KC_ORANGE:
                    *hsv = HSV_RED_ORANGE;
                    return true;
                case KC_GRAY:
                    *hsv = HSV_NONE;
                    return true;
                case KC_WHITE:
                    // Let the RGB mode be used on all keys, so we can preview it
                    return false;
            }
            break;
        case RLM_MODE: ;
            uint8_t rgb_mode = 0;
            led_on = _try_get_rgb_mode_from_key_pos(key_pos, &rgb_mode);
            key_is_current_val = rgb_mode == rgb_matrix_get_mode();
            hsv->s = 255;
            hsv->v = 255;
            break;
        case RLM_SPEED: ;
            // Alter the val value for speed
            led_on = _try_get_byte_from_key_pos(key_pos, &hsv->v);
            key_is_current_val = hsv->v == rgb_matrix_get_speed();
            hsv->s = 255;
            break;
        case RLM_HUE: ;
            led_on = _try_get_hue_from_key_pos(key_pos, &hsv->h);
            key_is_current_val = hsv->h == orig_hsv.h;
            break;
        case RLM_SAT: ;
            led_on = _try_get_byte_from_key_pos(key_pos, &hsv->s);
            key_is_current_val = hsv->s == orig_hsv.s;
            break;
        case RLM_VAL: ;
            led_on = _try_get_byte_from_key_pos(key_pos, &hsv->v);
            key_is_current_val = hsv->v == orig_hsv.v;
            break;
    }

    if (!led_on) {
        *hsv = HSV_NONE;
    }
    else if (key_is_current_val) {
        *hsv = pulse_hsv(*hsv, 512);
    }

    return true;
}

bool process_record_rgb_layer(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case CKC_RLM_MODE:
            return _process_ckc_rlm(record, RLM_MODE);
        case CKC_RLM_SPEED:
            return _process_ckc_rlm(record, RLM_SPEED);
        case CKC_RLM_HUE:
            return _process_ckc_rlm(record, RLM_HUE);
        case CKC_RLM_SAT:
            return _process_ckc_rlm(record, RLM_SAT);
        case CKC_RLM_VAL:
            return _process_ckc_rlm(record, RLM_VAL);
    }

    // From here on, only process keydown events
    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        case CKC_RGB_TOG:
            rgb_matrix_toggle_noeeprom();
            return false;
        case CKC_RGB_SAVE:
            eeconfig_update_rgb_matrix();
            return false;
        case CKC_RGB_LOAD:
            stolen_eeconfig_read_rgb_matrix();
            return false;
        case CKC_MODE_INC:
            rgb_matrix_step_noeeprom();
            return false;
        case CKC_MODE_DEC:
            rgb_matrix_step_reverse_noeeprom();
            return false;
        case CKC_SPEED_INC:
            rgb_matrix_set_speed_noeeprom(_inc_edit_byte(rgb_matrix_get_speed()));
            return false;
        case CKC_SPEED_DEC:
            rgb_matrix_set_speed_noeeprom(_dec_edit_byte(rgb_matrix_get_speed()));
            return false;
        case CKC_HUE_INC:
            rgb_matrix_set_hue_noeeprom(_inc_edit_hue(rgb_matrix_get_hue()));
            return false;
        case CKC_HUE_DEC:
            rgb_matrix_set_hue_noeeprom(_dec_edit_hue(rgb_matrix_get_hue()));
            return false;
        case CKC_SAT_INC:
            rgb_matrix_set_sat_noeeprom(_inc_edit_byte(rgb_matrix_get_sat()));
            return false;
        case CKC_SAT_DEC:
            rgb_matrix_set_sat_noeeprom(_dec_edit_byte(rgb_matrix_get_sat()));
            return false;
        case CKC_VAL_INC:
            rgb_matrix_set_val_noeeprom(_inc_edit_byte(rgb_matrix_get_val()));
            return false;
        case CKC_VAL_DEC:
            rgb_matrix_set_val_noeeprom(_dec_edit_byte(rgb_matrix_get_val()));
            return false;
    }

    switch (_rgb_layer_mode) {
        case RLM_PREVIEW:
            break;
        case RLM_MODE: ;
            uint8_t rgb_mode = 0;
            if (_try_get_rgb_mode_from_key_pos(record->event.key, &rgb_mode)) {
                rgb_matrix_mode_noeeprom(rgb_mode);
                _handle_rlm_key_press();
                return false;
            }
            break;
        case RLM_SPEED: ;
            uint8_t new_speed = 0;
            if (_try_get_byte_from_key_pos(record->event.key, &new_speed)) {
                rgb_matrix_set_speed_noeeprom(new_speed);
                _handle_rlm_key_press();
                return false;
            }
            break;
        case RLM_HUE: ;
            uint8_t new_hue = 0;
            if (_try_get_hue_from_key_pos(record->event.key, &new_hue)) {
                rgb_matrix_set_hue_noeeprom(new_hue);
                _handle_rlm_key_press();
                return false;
            }
            break;
        case RLM_SAT: ;
            uint8_t new_sat = 0;
            if (_try_get_byte_from_key_pos(record->event.key, &new_sat)) {
                rgb_matrix_set_sat_noeeprom(new_sat);
                _handle_rlm_key_press();
                return false;
            }
            break;
        case RLM_VAL: ;
            uint8_t new_val = 0;
            if (_try_get_byte_from_key_pos(record->event.key, &new_val)) {
                rgb_matrix_set_val_noeeprom(new_val);
                _handle_rlm_key_press();
                return false;
            }
            break;
    }

    return true;
}

layer_state_t layer_state_set_rgb_layer(layer_state_t state) {
    // Since we are editing the RGB lighting, ensure its turned on
    rgb_matrix_enable_noeeprom();

    // Always start in preview mode
    _rgb_layer_mode = RLM_PREVIEW;

    return state;
}
