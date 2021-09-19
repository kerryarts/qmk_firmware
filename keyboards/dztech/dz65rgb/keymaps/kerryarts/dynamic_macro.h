#pragma once

#include "action.h"
#include "color.h"
#include "keyboard.h"
#include <stdint.h>

enum macro_state {
    MS_NONE = 1,
    MS_RECORDING,
    MS_RECORDING_FULL,
    MS_PLAYING
};

bool process_led_macro(uint8_t led_index, keypos_t key_pos, uint16_t key_code, HSV* hsv);
bool process_record_macro(uint16_t keycode, keyrecord_t* record);
