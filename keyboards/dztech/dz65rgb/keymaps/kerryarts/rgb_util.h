#pragma once

#include "color.h"
#include <stdint.h>

#define HUE_INC 9  // Roughly matches the divisions used by the EDIT HUE

extern const HSV HSV_NONE;
extern const HSV HSV_RED_ORANGE;

HSV pulse_hsv(HSV hsv);
HSV inc_hsv(HSV hsv);
HSV dec_hsv(HSV hsv);

void stolen_eeconfig_read_rgb_matrix(void);
void rgb_matrix_set_hue_noeeprom(uint8_t hue);
void rgb_matrix_set_sat_noeeprom(uint8_t sat);
void rgb_matrix_set_val_noeeprom(uint8_t val);

void keyboard_post_init_rgb_util(void);
