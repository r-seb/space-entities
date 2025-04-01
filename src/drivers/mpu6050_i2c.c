#include "mpu6050_i2c.h"
#include "TM4C123GH6PM.h"
#include "printf.h"
#include <stdint.h>

static i2c_write_handler i2c_write;
static i2c_read_handler i2c_read;

void mpu6050_init(i2c_write_handler write_handler, i2c_read_handler read_handler)
{
    i2c_write = write_handler;
    i2c_read = read_handler;

    // Sanity check
    uint8_t who_am_i[2] = {WHO_AM_I, 0x00};
    (*i2c_read)(0x68, who_am_i, 2);
    printf_("I am MPU6050: %X\n\r", who_am_i[1]);
}
