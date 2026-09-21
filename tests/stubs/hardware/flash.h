#pragma once
#include <stddef.h>
#include <stdint.h>
#define FLASH_PAGE_SIZE 256u
void nuke_flash_do_cmd(const uint8_t *tx, uint8_t *rx, size_t count);
void flash_range_erase(uint32_t offset, size_t size);
void flash_range_program(uint32_t offset, const uint8_t *data, size_t size);
