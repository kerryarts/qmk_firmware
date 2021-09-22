#pragma once

#include "quantum_keycodes.h"

#define LED_INDEX_CAPS_LOCK 30

enum custom_layers {
    CL_BASE,
    CL_FUNC,
    CL_RGB,
    CL_SYS
};

enum custom_keycodes {
    CKC_RLM_MODE = SAFE_RANGE,
    CKC_RLM_SPEED,
    CKC_RLM_HUE,
    CKC_RLM_SAT,
    CKC_RLM_VAL,

    CKC_RGB_TOG,
    CKC_RGB_SAVE,
    CKC_RGB_LOAD,
    CKC_MODE_INC,
    CKC_MODE_DEC,
    CKC_SPEED_INC,
    CKC_SPEED_DEC,
    CKC_HUE_INC,
    CKC_HUE_DEC,
    CKC_SAT_INC,
    CKC_SAT_DEC,
    CKC_VAL_INC,
    CKC_VAL_DEC,

    CKC_WPM_TOG
};
