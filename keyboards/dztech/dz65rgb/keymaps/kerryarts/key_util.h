#pragma once

#include "keyboard.h"
#include <stdbool.h>
#include <stdint.h>

enum key_cap_color {
    KC_ORANGE = 1,
    KC_GRAY,
    KC_WHITE
};

typedef struct {
    enum key_cap_color cap_color;
    keypos_t pos;
    uint16_t code;
    bool is_mapped;
    bool is_layer_key;
    bool is_standard_key;
    bool is_macro_key;
} key_info;

enum key_cap_color get_key_cap_color(uint8_t led_index);
bool is_key_code_shiftable(uint16_t key_code);
bool is_key_code_mapped(uint16_t key_code);
bool is_key_code_func(uint16_t key_code);
bool is_key_code_macro(uint16_t key_code);
bool is_key_code_layer(uint16_t key_code);
