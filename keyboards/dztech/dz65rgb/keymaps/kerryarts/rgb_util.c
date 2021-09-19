#include "rgb_util.h"

#include "rgb_matrix.h"
#include "color.h"
#include <stdint.h>

const HSV HSV_NONE = {.h = 0, .s = 0, .v = 0};
const HSV HSV_RED_ORANGE = {.h = 21, .s = 255, .v = 255};

static uint16_t _pulse_timer = 0;

HSV pulse_hsv(HSV hsv) {
    uint16_t tick = timer_elapsed(_pulse_timer) % 512;

    // Over ~1/4 second
    uint8_t val = tick < 256
        ? tick // Increase the val from 0 to 255
        : 255 - (tick - 256); // Then decrease it from 255 to 0

    // Force max saturation for visability
    return (HSV) { .h = hsv.h, .s = 255, .v = val };
}

HSV inc_hsv(HSV hsv) {
    return (HSV) { .h = (hsv.h + HUE_INC * 2) % 256, .s = hsv.s, .v = hsv.v };
}

HSV dec_hsv(HSV hsv) {
    return (HSV) { .h = (hsv.h + 256 - (HUE_INC * 2)) % 256, .s = hsv.s, .v = hsv.v };
}

// Stolen from rgb_matrix.c
void stolen_eeconfig_read_rgb_matrix(void) {
    eeprom_read_block(&rgb_matrix_config, EECONFIG_RGB_MATRIX, sizeof(rgb_matrix_config));
}

void rgb_matrix_set_hue_noeeprom(uint8_t hue) {
    HSV curr_hsv = rgb_matrix_get_hsv();
    rgb_matrix_sethsv_noeeprom(hue, curr_hsv.s, curr_hsv.v);
}

void rgb_matrix_set_sat_noeeprom(uint8_t sat) {
    HSV curr_hsv = rgb_matrix_get_hsv();
    rgb_matrix_sethsv_noeeprom(curr_hsv.h, sat, curr_hsv.v);
}

void rgb_matrix_set_val_noeeprom(uint8_t val) {
    HSV curr_hsv = rgb_matrix_get_hsv();
    rgb_matrix_sethsv_noeeprom(curr_hsv.h, curr_hsv.s, val);
}



/*** KEYBOARD HOOKS - RGB UTIL ***/

void keyboard_post_init_rgb_util(void) {
    _pulse_timer = timer_read();
}
