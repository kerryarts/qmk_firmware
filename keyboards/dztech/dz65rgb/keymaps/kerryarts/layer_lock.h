#pragma once

#include "action.h"
#include "action_layer.h"
#include <stdint.h>
#include <stdbool.h>

bool process_record_layer_lock(uint16_t keycode, keyrecord_t* record);
layer_state_t layer_state_set_layer_lock(layer_state_t state);
