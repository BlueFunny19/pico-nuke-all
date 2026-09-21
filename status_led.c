// SPDX-License-Identifier: BSD-3-Clause
#include "status_led.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

#ifdef PICO_DEFAULT_WS2812_PIN
#include "hardware/clocks.h"
#include "hardware/pio.h"

// 800 kHz, 10 PIO cycles per bit; the output stalls low between pixels.
static const uint16_t pixel_instructions[] = {
    0x6321, // out x, 1     side 0 [3]
    0x1223, // jmp !x, 3    side 1 [2]
    0x1200, // jmp 0        side 1 [2]
    0xa242, // nop          side 0 [2]
};
static const struct pio_program pixel_program = {
    .instructions = pixel_instructions, .length = 4, .origin = -1,
};
static uint pixel_sm;
static void led_init(void) {
    pixel_sm = pio_claim_unused_sm(pio0, true);
    uint offset = pio_add_program(pio0, &pixel_program);
    pio_gpio_init(pio0, PICO_DEFAULT_WS2812_PIN);
    pio_sm_set_consecutive_pindirs(pio0, pixel_sm, PICO_DEFAULT_WS2812_PIN, 1, true);
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset, offset + 3);
    sm_config_set_sideset(&c, 1, false, false);
    sm_config_set_sideset_pins(&c, PICO_DEFAULT_WS2812_PIN);
    sm_config_set_out_shift(&c, false, true, 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    sm_config_set_clkdiv(&c, clock_get_hz(clk_sys) / 8000000.0f);
    pio_sm_init(pio0, pixel_sm, offset, &c);
    pio_sm_set_enabled(pio0, pixel_sm, true);
}
static void led_red(uint8_t level) {
#ifdef WAVESHARE_RP2350_ONE
    // This board uses RGB byte order; most other WS2812 boards use GRB.
    pio_sm_put_blocking(pio0, pixel_sm, (uint32_t)level << 24);
#else
    pio_sm_put_blocking(pio0, pixel_sm, (uint32_t)level << 16);
#endif
}
static void led_latch(void) {
    while (!pio_sm_is_tx_fifo_empty(pio0, pixel_sm)) tight_loop_contents();
    // Complete the final 24-bit word and the WS2812 reset/latch interval.
    busy_wait_us_32(350);
}
#elif defined(PICO_DEFAULT_LED_PIN)
#include "hardware/pwm.h"
static void led_init(void) {
    gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);
    pwm_config c = pwm_get_default_config();
    pwm_config_set_wrap(&c, 255);
    pwm_config_set_clkdiv(&c, 125.0f);
#ifdef PICO_DEFAULT_LED_PIN_INVERTED
    pwm_config_set_output_polarity(&c, true, true);
#endif
    pwm_init(slice, &c, true);
}
static void led_red(uint8_t level) { pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, level); }
static void led_latch(void) {}
#else
static void led_init(void) {}
static void led_red(uint8_t level) { (void)level; }
static void led_latch(void) {}
#endif

enum { LED_READY = 1, LED_COMPLETE, LED_ERROR };

// Both cores, their constants and the LED driver run from SRAM. Core 1 keeps
// breathing throughout a blocking flash erase without relying on flash or IRQs.
static void led_core(void) {
    led_init();
    led_red(NUKE_LED_PEAK);
    multicore_fifo_push_blocking(LED_READY);
    uint32_t started = time_us_32();
    nuke_led_state_t state = NUKE_LED_ERASING;
    for (;;) {
        if (multicore_fifo_rvalid()) {
            uint32_t command = multicore_fifo_pop_blocking();
            if (command == LED_COMPLETE) {
                led_red(nuke_led_level(NUKE_LED_COMPLETE, 0));
                led_latch();
                multicore_fifo_push_blocking(LED_COMPLETE);
                for (;;) tight_loop_contents();
            }
            if (command == LED_ERROR) state = NUKE_LED_ERROR;
        }
        led_red(nuke_led_level(state, (time_us_32() - started) / 1000u));
        busy_wait_ms(20);
    }
}
void status_led_start(void) {
    multicore_launch_core1(led_core);
    hard_assert(multicore_fifo_pop_blocking() == LED_READY);
}
void status_led_complete(void) {
    multicore_fifo_push_blocking(LED_COMPLETE);
    hard_assert(multicore_fifo_pop_blocking() == LED_COMPLETE);
    // No new pixel data after the final red latch, including during ROM reboot.
    multicore_reset_core1();
}
void status_led_error(void) {
    multicore_fifo_push_blocking(LED_ERROR);
}
