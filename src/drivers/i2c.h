#pragma once

#include <stdint.h>

// Used I2C Module 1 at GPIOA
#define SCL_PIN (1U << 6)
#define SDA_PIN (1U << 7)

typedef void (*i2c_write_handler)(uint8_t slave_addr, uint8_t* buffer, uint8_t buf_idx);

void i2c1_init(void);
void i2c1_write(uint8_t slave_addr, uint8_t* buffer, uint8_t buf_idx);