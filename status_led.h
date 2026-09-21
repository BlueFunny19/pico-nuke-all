// SPDX-License-Identifier: BSD-3-Clause
#ifndef NUKE_STATUS_LED_H
#define NUKE_STATUS_LED_H
#include <stdint.h>

#define NUKE_WARNING_MS 4000u
#define NUKE_LED_PEAK 32u

typedef enum {
    NUKE_LED_ERASING,
    NUKE_LED_COMPLETE,
    NUKE_LED_ERROR,
} nuke_led_state_t;

// Two-second smooth red breathing, fast red error blinking, or steady red.
static inline uint8_t nuke_led_level(nuke_led_state_t state, uint32_t elapsed_ms) {
    if (state == NUKE_LED_COMPLETE) return NUKE_LED_PEAK;
    if (state == NUKE_LED_ERROR) return (elapsed_ms / 125u) % 2u ? 0 : NUKE_LED_PEAK;
    uint32_t phase = elapsed_ms % 2000u;
    uint32_t ramp = phase < 1000u ? 1000u - phase : phase - 1000u;
    return (uint8_t)(((uint64_t)NUKE_LED_PEAK * ramp * ramp * (3000u - 2u * ramp)
                      + 500000000u) / 1000000000u);
}

void status_led_start(void);
void status_led_complete(void);
void status_led_error(void);
#endif
