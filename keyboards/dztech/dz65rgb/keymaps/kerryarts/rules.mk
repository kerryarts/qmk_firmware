# Console for debug. If 'yes', should un-disable DEBUG and PRINT in config.h too
CONSOLE_ENABLE = no

# Enables Link Time Optimization (LTO) when compiling the keyboard
# This makes the process take longer, but it can significantly reduce the compiled size
LTO_ENABLE = yes

# Optional features
DYNAMIC_MACRO_ENABLE = yes
TAP_DANCE_ENABLE = yes
# WPM_ENABLE = yes # use home grown WPM feature for now, seems more accurate

# Optional features - disabled to reduce firmware size
NKRO_ENABLE = no
EXTRAFLAGS += -flto
MOUSEKEY_ENABLE = no
SPACE_CADET_ENABLE = no
UNICODEMAP_ENABLE = no
UNICODE_ENABLE = no
SPLIT_KEYBOARD = no
COMMAND_ENABLE = no
SWAP_HANDS_ENABLE = no
COMBO_ENABLE = no

# Source files
SRC += key_util.c
SRC += rgb_util.c
SRC += rgb_layer.c
SRC += dynamic_macro.c
SRC += tap_dance.c
SRC += wpm_vu.c
SRC += custom_layers.c
