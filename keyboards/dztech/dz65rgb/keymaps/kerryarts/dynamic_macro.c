#include "dynamic_macro.h"
#include "rgb_util.h"
#include "key_util.h"

#include "action.h"
#include "quantum_keycodes.h"
#include <stdbool.h>

#define LED_INDEX_MACRO_REC_1 26
#define LED_INDEX_MACRO_REC_2 40
#define LED_INDEX_MACRO_PLAY_1 27
#define LED_INDEX_MACRO_PLAY_2 41

static enum macro_state _macro_state = MS_NONE;
static bool _is_macro_1 = false;



/*** KEYBOARD HOOKS - MACRO ***/

bool process_led_macro(uint8_t led_index, keypos_t key_pos, uint16_t key_code, HSV* hsv) {
    switch (_macro_state) {
        case MS_NONE:
            return false;
        case MS_RECORDING:
            if ((led_index == LED_INDEX_MACRO_REC_1 && _is_macro_1) || (led_index == LED_INDEX_MACRO_REC_2 && !_is_macro_1)) {
                *hsv = pulse_hsv(dec_hsv(*hsv));
                return true;
            }
            break;
        case MS_RECORDING_FULL:
            if ((led_index == LED_INDEX_MACRO_REC_1 && _is_macro_1) || (led_index == LED_INDEX_MACRO_REC_2 && !_is_macro_1)) {
                *hsv = dec_hsv(*hsv);
                return true;
            }
            break;
        case MS_PLAYING:
            if ((led_index == LED_INDEX_MACRO_PLAY_1 && _is_macro_1) || (led_index == LED_INDEX_MACRO_PLAY_2 && !_is_macro_1)) {
                *hsv = dec_hsv(*hsv);
                return true;
            }
            break;
    }

    // If recording or playing back a macro, and this key wasn't considered 'active' above, turn it off
    if (_macro_state != MS_NONE && is_key_code_macro(key_code)) {
        *hsv = HSV_NONE;
        return true;
    }

    return false;
}

bool process_record_macro(uint16_t keycode, keyrecord_t* record) {
    // Only process keydown events
    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        case DYN_REC_START1:
            _is_macro_1 = true;
            _macro_state = MS_RECORDING;
            break;
        case DYN_REC_START2:
            _is_macro_1 = false;
            _macro_state = MS_RECORDING;
            break;
        case DYN_MACRO_PLAY1:
            _is_macro_1 = true;
            _macro_state = MS_PLAYING;
            break;
        case DYN_MACRO_PLAY2:
            _is_macro_1 = false;
            _macro_state = MS_PLAYING;
            break;
    }

    // We still want the macro to get processed
    return true;
}

void dynamic_macro_record_start_user(void) {
    // Handled in process_record_user() instead
}

// Called when the macro recording has ended
void dynamic_macro_record_end_user(int8_t direction) {
    _macro_state = MS_NONE;
}

// Called when the macro has finished playing
void dynamic_macro_play_user(int8_t direction) {
    _macro_state = MS_NONE;
}

// Called when the macro is full
void dynamic_macro_record_key_user(int8_t direction, keyrecord_t *record) {
    _macro_state = MS_RECORDING_FULL;
}
