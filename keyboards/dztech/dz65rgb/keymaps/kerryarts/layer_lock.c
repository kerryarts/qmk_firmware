#include "layer_lock.h"

#include "custom_layers.h"

#include "action.h"
#include "keyboard.h"
#include "action_layer.h"
#include <stdint.h>
#include <stdbool.h>

// Zero means no lock. Locking layer zero is not supported or needed.
static layer_state_t _locked_layer_state = 0;

bool process_record_layer_lock(uint16_t keycode, keyrecord_t* record) {
    // Only process keydown events
    if (!record->event.pressed) {
        return true;
    }

    uint8_t layer_num = 0;
    switch (keycode) {
        // Easier to just hardcode the layer num instead of using a mask, for now
        case CKC_LL_1:
            layer_num = 1;
            break;
        default:
            return true;
    }

    layer_state_t layer_mask = (layer_state_t) 1 << layer_num;

    // If this keys layer is currently locked on, toggle it off
    if ((_locked_layer_state & layer_mask) > 0) {
        _locked_layer_state &= ~layer_mask;
        layer_off(layer_num);
    }
    // Else toggle it on
    else {
        _locked_layer_state |= layer_mask;
        layer_on(layer_num);
    }

    return false;
}

layer_state_t layer_state_set_layer_lock(layer_state_t state) {
    return state | _locked_layer_state;
}
