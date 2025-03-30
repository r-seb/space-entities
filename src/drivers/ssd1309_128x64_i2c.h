#pragma once

#include "i2c.h"
#include "u8g2.h"
#include <stdint.h>

void ssd1309_128x64_init(u8g2_t* oled, i2c_write_handler write_handler);
uint8_t u8x8_gpio_and_delay_i2c(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t u8x8_i2c(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);