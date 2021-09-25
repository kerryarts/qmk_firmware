/*
 * Features that can be enabled
 */

// TODO: This probably removes the need to have NKRO keys mapped
// #define FORCE_NKRO



/*
 * Features that cab be disabled (to reduce firmware size)
 */

#ifndef NO_DEBUG
#   define NO_DEBUG
#endif // !NO_DEBUG
#if !defined(NO_PRINT) && !defined(CONSOLE_ENABLE)
#   define NO_PRINT
#endif // !NO_PRINT

#define DISABLE_LEADER

// Disable layers (we use these)
// #define NO_ACTION_LAYER

// Disable tap dance and other tapping features (we use these)
// #define NO_ACTION_TAPPING

// Disable one - shot modifiers (we use these)
// #define NO_ACTION_ONESHOT

// Disable old style macro handling (already disabled in keyboard rules.mk)
// #define NO_ACTION_MACRO

// Disable old style function handling (already disabled in keyboard rules.mk)
// #define NO_ACTION_FUNCTION



/*
 * Macro settings
 */

// Do not allow one macro to contain the other macro
#define DYNAMIC_MACRO_NO_NESTING

// This normally defaults to 128, but it was causing issues on this board. TODO: Investigate.
// Note that the number of keypresses is actually half this (1 each for keydown and keyup)
#define DYNAMIC_MACRO_SIZE 64



/*
 * Tap settings
 */

// Makes tap and hold keys trigger the hold if another key is pressed before releasing, even if it hasn't hit the `TAPPING_TERM`
#define PERMISSIVE_HOLD

// How many taps before triggering a layer toggle via TT()
#define TAPPING_TOGGLE 2

// Tapping this number of times holds the key until tapped once again
#define ONESHOT_TAP_TOGGLE 2

// How long (in ms) before a tap becomes a hold. Default 200.
#define TAPPING_TERM 150



/*
 * Special keycode settings
 */

// The GraceEsc keycode makes Shift+Esc output '~', however this breaks the Ctrl+Shift+Esc shortcut for task manager in Windows
// These defines revert Shift+Esc to default behaviour when Alt or Ctrl is held, which fixes this
#define GRAVE_ESC_ALT_OVERRIDE
#define GRAVE_ESC_CTRL_OVERRIDE



/*
 * RGB settings
 */

// Required for DIGITAL_RAIN and TYPING_HEATMAP
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS

// Change centre of radial effects to be centred further left within the alpha keys. See g_led_config for positions.
// Original: { 112, 32 }, centered on B: { 94, 32 }, centered on H: { 101, 32 }
#define RGB_MATRIX_CENTER { 101, 32 }

// Set default animation mode
#undef RGB_MATRIX_STARTUP_MODE
#define RGB_MATRIX_STARTUP_MODE RGB_MATRIX_HUE_BREATHING

// Key [Z] of the EDIT HUE keys
#define RGB_MATRIX_STARTUP_HUE 8

// Key [7] of the EDIT BYTE keys
#define RGB_MATRIX_STARTUP_SPD 139

// We use our own custom key codes for RGB. Free up firmware space by disabling the built in ones.
#define RGB_MATRIX_DISABLE_KEYCODES

/*
 * RGB effects
 */

// Free up firmware space by disabling the RGB modes which are less interesting
// #define DISABLE_RGB_MATRIX_SOLID_COLOR            //
#define DISABLE_RGB_MATRIX_ALPHAS_MODS               // Static dual hue, speed is hue for secondary hue
// #define DISABLE_RGB_MATRIX_GRADIENT_UP_DOWN          // Static gradient top to bottom, speed controls how much gradient changes
// #define DISABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT       // Static gradient left to right, speed controls how much gradient changes
#define DISABLE_RGB_MATRIX_BREATHING                 // Single hue brightness cycling animation
#define DISABLE_RGB_MATRIX_BAND_SAT                  // Single hue band fading saturation scrolling left to right
#define DISABLE_RGB_MATRIX_BAND_VAL                  // Single hue band fading brightness scrolling left to right
#define DISABLE_RGB_MATRIX_BAND_PINWHEEL_SAT         // Single hue 3 blade spinning pinwheel fades saturation
#define DISABLE_RGB_MATRIX_BAND_PINWHEEL_VAL         // Single hue 3 blade spinning pinwheel fades brightness
#define DISABLE_RGB_MATRIX_BAND_SPIRAL_SAT           // Single hue spinning spiral fades saturation
#define DISABLE_RGB_MATRIX_BAND_SPIRAL_VAL           // Single hue spinning spiral fades brightness
#define DISABLE_RGB_MATRIX_CYCLE_ALL                 // Full keyboard solid hue cycling through full gradient
#define DISABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT          // Full gradient scrolling left to right
// #define DISABLE_RGB_MATRIX_CYCLE_UP_DOWN             // Full gradient scrolling top to bottom
// #define DISABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON    // Full gradent Chevron shapped scrolling left to right
// #define DISABLE_RGB_MATRIX_CYCLE_OUT_IN              // Full gradient scrolling out to in
// #define DISABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL         // Full dual gradients scrolling out to in
#define DISABLE_RGB_MATRIX_CYCLE_PINWHEEL            // Full gradient spinning pinwheel around center of keyboard
// #define DISABLE_RGB_MATRIX_CYCLE_SPIRAL              // Full gradient spinning spiral around center of keyboard
// #define DISABLE_RGB_MATRIX_DUAL_BEACON               // Full gradient spinning around center of keyboard
#define DISABLE_RGB_MATRIX_RAINBOW_BEACON            // Full tighter gradient spinning around center of keyboard
#define DISABLE_RGB_MATRIX_RAINBOW_PINWHEELS         // Full dual gradients spinning two halfs of keyboard
#define DISABLE_RGB_MATRIX_RAINDROPS                 // Randomly changes a single key's hue
#define DISABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS       // Randomly changes a single key's hue and saturation
// #define DISABLE_RGB_MATRIX_HUE_BREATHING             // Hue shifts up a slight ammount at the same time, then shifts back
#define DISABLE_RGB_MATRIX_HUE_PENDULUM              // Hue shifts up a slight ammount in a wave to the right, then back to the left
#define DISABLE_RGB_MATRIX_HUE_WAVE                  // Hue shifts up a slight ammount and then back down in a wave to the right
#define DISABLE_RGB_MATRIX_DIGITAL_RAIN              // That famous computer simulation
// #define DISABLE_RGB_MATRIX_TYPING_HEATMAP            // How hot is your WPM!
#define DISABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE     // Pulses keys hit to hue & value then fades value outS
#define DISABLE_RGB_MATRIX_SOLID_REACTIVE            // Static single hue, pulses keys hit to shifted hue then fades to current hue
#define DISABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE       // Hue & value pulse near a single key hit then fades value out
// #define DISABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE  // Hue & value pulse near multiple key hits then fades value out
#define DISABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS      // Hue & value pulse the same column and row of a single key hit then fades value out
// #define DISABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS // Hue & value pulse the same column and row of multiple key hits then fades value out
#define DISABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS      // Hue & value pulse away on the same column and row of a single key hit then fades value out
// #define DISABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS // Hue & value pulse away on the same column and row of multiple key hits then fades value out
#define DISABLE_RGB_MATRIX_SPLASH                    // Full gradient & value pulse away from a single key hit then fades value out
// #define DISABLE_RGB_MATRIX_MULTISPLASH               // Full gradient & value pulse away from multiple key hits then fades value out
#define DISABLE_RGB_MATRIX_SOLID_SPLASH              // Hue & value pulse away from a single key hit then fades value out
// #define DISABLE_RGB_MATRIX_SOLID_MULTISPLASH         // Hue & value pulse away from multiple key hits then fades value out
