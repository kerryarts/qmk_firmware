#pragma once

#include "color.h"
#include <stdint.h>

#define HUE_INC 9  // Roughly matches the divisions used by the EDIT HUE
#define BYTE_INC 21  // Roughly matches the divisions used by the EDIT BYTE

enum rgb_mode_flags {
    RMF_HUE_SINGLE = 1,
    RMF_HUE_MULTI  = 2,
    RMF_STYLE_STAIC = 4,
    RMF_STYLE_ANIM = 8,
    RMF_STLYE_REACTIVE = 16
};

extern const HSV HSV_NONE;
extern const HSV HSV_RED_ORANGE;

HSV pulse_hsv(HSV hsv, uint16_t duration);
HSV inc_hsv(HSV hsv);
HSV dec_hsv(HSV hsv);
HSV inv_hsv(HSV hsv);

enum rgb_mode_flags get_rgb_mode_flags(uint8_t rgb_mode);

void stolen_eeconfig_read_rgb_matrix(void);
void rgb_matrix_set_hue_noeeprom(uint8_t hue);
void rgb_matrix_set_sat_noeeprom(uint8_t sat);
void rgb_matrix_set_val_noeeprom(uint8_t val);

void keyboard_post_init_rgb_util(void);
