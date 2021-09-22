#pragma once

#include "action.h"
#include "keyboard.h"
#include "color.h"
#include <stdbool.h>

uint8_t get_curr_wpm(void);
void keyboard_post_init_wpm(void);
void matrix_scan_wpm(void);
bool process_led_wpm(uint8_t led_index, keypos_t key_pos, uint16_t key_code, HSV * hsv);
bool process_record_wpm(uint16_t keycode, keyrecord_t * record);
