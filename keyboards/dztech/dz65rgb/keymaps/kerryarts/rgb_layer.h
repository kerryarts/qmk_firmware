#pragma once

#include "action.h"
#include "action_layer.h"
#include "color.h"
#include "keyboard.h"
#include <stdint.h>
#include <stdbool.h>

#define EDIT_HUE_KEY_COUNT 30
#define EDIT_BYTE_KEY_COUNT 12

enum rgb_layer_mode {
    RLM_PREVIEW = 1,
    RLM_MODE,
    RLM_SPEED,
    RLM_HUE,
    RLM_SAT,
    RLM_VAL
};

bool process_led_rgb_layer(uint8_t led_index, keypos_t key_pos, uint16_t key_code, HSV* hsv);
bool process_record_rgb_layer(uint16_t keycode, keyrecord_t* record);
layer_state_t layer_state_set_rgb_layer(layer_state_t state);
