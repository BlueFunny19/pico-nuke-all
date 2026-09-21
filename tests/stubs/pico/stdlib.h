#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define PICO_NO_FLASH 1
typedef unsigned int uint;
extern uint8_t fake_flash[];
#define XIP_BASE ((uintptr_t)fake_flash)
void sleep_ms(uint32_t ms);
void tight_loop_contents(void);
