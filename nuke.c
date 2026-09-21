/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Obliterate the contents of flash. This is a silly thing to do if you are
// trying to run this program from flash, so you should really load and run
// directly from SRAM. You can enable RAM-only builds for all targets by doing:
//
// cmake -DPICO_NO_FLASH=1 ..
//
// in your build directory. We've also forced no-flash builds for this app in
// particular by adding:
//
// pico_set_binary_type(flash_nuke no_flash)
//
// To the CMakeLists.txt app for this file. Just to be sure, we can check the
// define:
#if !PICO_NO_FLASH && !PICO_COPY_TO_RAM
#error "This example must be built to run from SRAM!"
#endif

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "pico/bootrom.h"

#include "status_led.h"
#include <string.h>

// RAM-safe JEDEC command shim in flash.c (also works with SDK 2.3 wrappers).
void nuke_flash_do_cmd(const uint8_t *tx, uint8_t *rx, size_t count);

static bool erased(uint32_t size) {
    const volatile uint8_t *flash = (const volatile uint8_t *)XIP_BASE;
    for (uint32_t i = 0; i < size; ++i) {
        if (flash[i] != 0xff) return false;
    }
    return true;
}
static void fail(void) {
    status_led_error();
    // Keep an error visibly distinct from verified completion.
    for (;;) tight_loop_contents();
}

int main(void) {
    status_led_start();
    // Visible warning before the first destructive operation.
    sleep_ms(NUKE_WARNING_MS);
    uint8_t txbuf[4] = {0x9f, 0, 0, 0};
    uint8_t rxbuf[4] = {0};
    nuke_flash_do_cmd(txbuf, rxbuf, sizeof(txbuf));
    // Bound the JEDEC shift and stay within the flash API's 32 MiB range.
#if PICO_RP2040
    const uint8_t max_capacity = 24; // RP2040's linear XIP window is 16 MiB.
#else
    const uint8_t max_capacity = 25;
#endif
    if (rxbuf[3] < 12 || rxbuf[3] > max_capacity) fail();
    uint32_t flash_size_bytes = 1u << rxbuf[3];
    flash_range_erase(0, flash_size_bytes);
    if (!erased(flash_size_bytes)) fail();

    // Preserve the existing picotool size marker in the first page.
    static const uint8_t eyecatcher[FLASH_PAGE_SIZE] = "NUKE";
    flash_range_program(0, eyecatcher, FLASH_PAGE_SIZE);
    if (memcmp((const void *)XIP_BASE, eyecatcher, sizeof(eyecatcher)) != 0) fail();

    status_led_complete();
    reset_usb_boot(0, 0);
}
