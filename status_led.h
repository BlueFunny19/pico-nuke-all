// SPDX-License-Identifier: BSD-3-Clause
#ifndef NUKE_STATUS_LED_H
#define NUKE_STATUS_LED_H
#include <stdint.h>

#define NUKE_LED_PEAK 32u

typedef enum {
    NUKE_LED_ERASING,
    NUKE_LED_COMPLETE,
    NUKE_LED_ERROR,
} nuke_led_state_t;

// Execution and verified completion are steady red; errors remain distinct.
static inline uint8_t nuke_led_level(nuke_led_state_t state, uint32_t elapsed_ms) {
    if (state == NUKE_LED_ERROR) return (elapsed_ms / 125u) % 2u ? 0 : NUKE_LED_PEAK;
    return NUKE_LED_PEAK;
}

void status_led_start(void);
void status_led_complete(void);
void status_led_error(void);
#endif
