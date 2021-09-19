#include "tap_dance.h"

#include "action.h"
#include "process_tap_dance.h"
#include "quantum.h"
#include "keycode.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t kc;
} tap_dance_shift_t;

#define ACTION_TAP_DANCE_SHIFT(kc) { .fn = { _tap_dance_shift_on_each_tap, _tap_dance_shift_finished, _tap_dance_shift_reset}, .user_data = (void *)&((tap_dance_shift_t){kc}), }

void _tap_dance_shift_on_each_tap(qk_tap_dance_state_t *state, void *user_data);
void _tap_dance_shift_finished(qk_tap_dance_state_t *state, void *user_data);
void _tap_dance_shift_reset(qk_tap_dance_state_t *state, void *user_data);

qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_LBRACKET] = ACTION_TAP_DANCE_SHIFT(KC_LBRACKET),
    [TD_RBRACKET] = ACTION_TAP_DANCE_SHIFT(KC_RBRACKET),
    [TD_SCOLON] = ACTION_TAP_DANCE_SHIFT(KC_SCOLON),
    [TD_QUOTE] = ACTION_TAP_DANCE_SHIFT(KC_QUOTE),
    [TD_COMMA] = ACTION_TAP_DANCE_SHIFT(KC_COMMA),
    [TD_DOT] = ACTION_TAP_DANCE_SHIFT(KC_DOT),
    [TD_SLASH] = ACTION_TAP_DANCE_SHIFT(KC_SLASH)
};

void _tap_dance_shift_on_each_tap(qk_tap_dance_state_t *state, void *user_data) {
    tap_dance_shift_t *shift = (tap_dance_shift_t *)user_data;

    if (state->count == 2) {
        register_code16(LSFT(shift->kc));
        state->finished = true;
    }
}

void _tap_dance_shift_finished(qk_tap_dance_state_t *state, void *user_data) {
    tap_dance_shift_t *shift = (tap_dance_shift_t *)user_data;

    if (state->count == 1) {
        register_code16(shift->kc);
    } else if (state->count == 2) {
        register_code16(LSFT(shift->kc));
    }
}

void _tap_dance_shift_reset(qk_tap_dance_state_t *state, void *user_data) {
    tap_dance_shift_t *shift = (tap_dance_shift_t *)user_data;

    if (state->count == 1) {
        unregister_code16(shift->kc);
    } else if (state->count == 2) {
        unregister_code16(LSFT(shift->kc));
    }
}
