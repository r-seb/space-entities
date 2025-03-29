#pragma once

#include "TM4C123GH6PM.h"
#include "u8g2.h"
#include <stdint.h>

// Used I2C Module 1 at GPIOA
#define SCL_PIN (1U << 6)
#define SDA_PIN (1U << 7)

void ssd1309_128x64_init(u8g2_t *oled);
void i2c1_init(void);
void i2c1_write(uint8_t slave_addr, uint8_t* buffer, uint8_t buf_idx);
uint8_t u8x8_gpio_and_delay_i2c(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t u8x8_i2c(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);