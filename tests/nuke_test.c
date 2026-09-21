// SPDX-License-Identifier: BSD-3-Clause
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "../status_led.h"

static jmp_buf finished;
uint8_t fake_flash[16384];
static uint8_t capacity;
static bool fail_erase, fail_program;
static unsigned erase_calls, program_calls, led_state;
#define main nuke_entry
#include "../nuke.c"
#undef main

void status_led_start(void) { led_state = 1; }
void status_led_complete(void) { assert(program_calls == 1); led_state = 2; }
void status_led_error(void) { led_state = 3; }
void sleep_ms(uint32_t ms) { (void)ms; assert(!"Nuke must not add another confirmation delay"); }
void tight_loop_contents(void) { assert(led_state == 3); longjmp(finished, 2); }
void reset_usb_boot(uint32_t activity, uint32_t disable) {
    assert(!activity && !disable && led_state == 2);
    longjmp(finished, 1);
}
void nuke_flash_do_cmd(const uint8_t *tx, uint8_t *rx, size_t count) {
    assert(led_state == 1);
    assert(count == 4 && tx[0] == 0x9f && tx[1] == 0 && tx[2] == 0 && tx[3] == 0);
    rx[3] = capacity;
}
void flash_range_erase(uint32_t offset, size_t size) {
    assert(led_state == 1 && offset == 0 && size == sizeof(fake_flash));
    erase_calls++;
    memset(fake_flash, 0xff, size);
    if (fail_erase) fake_flash[size - 1] = 0;
}
void flash_range_program(uint32_t offset, const uint8_t *data, size_t size) {
    assert(led_state == 1 && erase_calls == 1 && offset == 0 && size == FLASH_PAGE_SIZE);
    program_calls++;
    if (!fail_program) memcpy(fake_flash, data, size);
}
static void run_case(uint8_t c, bool bad_erase, bool bad_program, bool success) {
    capacity = c;
    fail_erase = bad_erase;
    fail_program = bad_program;
    erase_calls = program_calls = led_state = 0;
    memset(fake_flash, 0x5a, sizeof(fake_flash));
    int result = setjmp(finished);
    if (!result) { nuke_entry(); assert(false); }
    assert(result == (success ? 1 : 2));
    if (c != 14) assert(erase_calls == 0 && program_calls == 0);
    if (bad_erase) assert(erase_calls == 1 && program_calls == 0);
    if (success) {
        assert(!memcmp(fake_flash, "NUKE", 4));
        for (size_t i = FLASH_PAGE_SIZE; i < sizeof(fake_flash); ++i) assert(fake_flash[i] == 0xff);
    }
}
int main(void) {
    for (uint32_t t = 0; t < 10000; t += 17) {
        assert(nuke_led_level(NUKE_LED_ERASING, t) == NUKE_LED_PEAK);
        assert(nuke_led_level(NUKE_LED_COMPLETE, t) == NUKE_LED_PEAK);
    }
    assert(nuke_led_level(NUKE_LED_ERROR, 0) == NUKE_LED_PEAK);
    assert(nuke_led_level(NUKE_LED_ERROR, 125) == 0);
    run_case(14, false, false, true);
    run_case(14, true, false, false);
    run_case(14, false, true, false);
    run_case(0, false, false, false);
    run_case(11, false, false, false);
    run_case(26, false, false, false);
    run_case(255, false, false, false);
#if PICO_RP2040
    run_case(25, false, false, false);
#endif
    puts("PASS steady execution/completion, error pattern, no extra delay, erase verification and invalid JEDEC handling");
}
