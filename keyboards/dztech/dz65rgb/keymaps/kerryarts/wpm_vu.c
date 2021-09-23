#include "wpm_vu.h"

#include "custom_layers.h"
#include "rgb_util.h"

#include "action.h"
#include "keyboard.h"
#include "color.h"
#include <stdbool.h>
#include <stdint.h>

#define SAMPLE_SEC 30
#define SAMPLE_ARRAY_SIZE SAMPLE_SEC + 1
#define WORD_SIZE 5

static bool _wpm_enabled = false;
static uint8_t _key_count_samples[SAMPLE_ARRAY_SIZE] = {0}; // Stores 1 sample per second, with 1 additional buffer sample
static uint16_t _wpm_timer = 0;
static uint16_t _current_key_count = 0; // Treated as a rolling average over the SAMPLE_SEC window

uint8_t _get_curr_sample_index(void) {
    uint16_t ellapsed_ms = timer_elapsed(_wpm_timer);
    return (ellapsed_ms / 1000) % SAMPLE_ARRAY_SIZE;
}

// Stolen from wpm.c
bool _is_wpm_keycode(uint16_t keycode) {
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) || (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX) || (keycode >= QK_MODS && keycode <= QK_MODS_MAX)) {
        keycode = keycode & 0xFF;
    } else if (keycode > 0xFF) {
        keycode = 0;
    }

    if ((keycode >= KC_A && keycode <= KC_0) || (keycode >= KC_TAB && keycode <= KC_SLASH)) {
        return true;
    }

    return false;
}

uint8_t get_curr_wpm(void) {
    if (!_wpm_enabled) {
        return 0;
    }

    uint16_t wpm = (_current_key_count / WORD_SIZE) * (60 / SAMPLE_SEC);
    if (wpm > 255) {
        return 255;
    }

    return wpm;
}

void keyboard_post_init_wpm(void) {
    _wpm_timer = timer_read();
}

void matrix_scan_wpm(void) {
    if (!_wpm_enabled) {
        return;
    }

    // Index represents the current sample. Index + 1 represents the oldest sample.
    uint8_t index = (_get_curr_sample_index() + 1) % SAMPLE_ARRAY_SIZE;
    uint8_t count = _key_count_samples[index];

    if (count == 0) {
        return;
    }

    // Remove the oldest sample from our rolling count
    _current_key_count -= count;

    // Clear the historical count. This index will be incremented in the next second.
    _key_count_samples[index] = 0;
}

bool process_led_wpm(uint8_t led_index, keypos_t key_pos, uint16_t key_code, HSV* hsv) {
    if (_wpm_enabled && key_pos.row == 0 && key_pos.col >= 1 && key_pos.col <= 12) {
        // Average person is around 40 WPM. 80 is good.
        uint8_t wpm = get_curr_wpm();
        uint8_t hue;

        // Keys [0] to [=]. WPM 100 to 129. 130 off the charts.
        if (key_pos.col >= 10) {
            hue = 0;
        }
        // Keys [7] to [9]. WPM 70 to 99.
        else if (key_pos.col >= 7) {
            hue = 43;
        }
        // Keys [1] to [6]. WPM 10 to 69.
        else {
            hue = 85;
        }

        // If the WPM is >= X5 for the current X key, pulse the light to indicate we are getting close to key X+1!
        if (wpm / 10 == key_pos.col && wpm % 10 >= 5) {
            *hsv = pulse_hsv((HSV){.h = hue, .s = 255, .v = 255}, 256);
        }
        // If this key is greater than the current WPM, light it solid
        else if (wpm >= key_pos.col * 10) {
            *hsv = (HSV){.h = hue, .s = 255, .v = 255};
        }
        // Else light it dim
        else {
            *hsv = (HSV){.h = hue, .s = 175, .v = 175};
        }

        return true;
    }

    return false;
}

bool process_record_wpm(uint16_t keycode, keyrecord_t* record) {
    if (_wpm_enabled && record->event.pressed && _is_wpm_keycode(keycode)) {
        uint8_t index = _get_curr_key_count_index();

        _current_key_count++;
        _key_count_by_second[index] += 1;
    }

    if (record->event.pressed && keycode == CKC_WPM_TOG) {
        _wpm_enabled = !_wpm_enabled;

        if (_wpm_enabled) {
            _current_key_count = 0;

            for (uint8_t i; i < SAMPLE_SEC; i++) {
                _key_count_by_second[i] = 0;
            }
        }

        return false;
    }

    return true;
}
