#include "mpu6050_i2c.h"
#include "TM4C123GH6PM.h"
#include "i2c.h"
#include "printf.h"
#include <stdint.h>

static i2c_write_handler i2c_write;
static i2c_read_handler i2c_read;

void mpu6050_init(i2c_write_handler write_handler, i2c_read_handler read_handler)
{
    i2c_write = write_handler;
    i2c_read = read_handler;

    // Enable interrupt, pg. 27
    uint8_t int_enable[2] = {INT_ENABLE, 0x01};
    (*i2c_write)(0x68, int_enable, 2);

    // Sanity check
    uint8_t who_am_i[2] = {WHO_AM_I, 0x00};
    (*i2c_read)(0x68, who_am_i, 2);
    printf_("I am MPU6050: %X\n\r", who_am_i[1]);

    // Sample Rate Divider, pg. 12
    // With DLPG enabled, Sample Rate = 1000 / (1 + 9) = 100Hz
    uint8_t smplrt_div[2] = {SMPLRT_DIV, 0x09};
    (*i2c_write)(0x68, smplrt_div, 2);

    // Config with DLPG, pg. 13
    uint8_t config[2] = {CONFIG, 0x02};
    (*i2c_write)(0x68, config, 2);

    // Set pwr_mgmt to wake the board, pg. 40
    uint8_t pwr_mgmt_1[2] = {PWR_MGMT_1, 0x04};
    (*i2c_write)(0x68, pwr_mgmt_1, 2);

    // Set gyro config, pg. 14, perform self test
    uint8_t gyro_config[2] = {GYRO_CONFIG, 0xE0};
    (*i2c_write)(0x68, gyro_config, 2);

    // Set accel config, pg. 15, perform self test
    uint8_t accel_config[2] = {ACCEL_CONFIG, 0xE0};
    (*i2c_write)(0x68, accel_config, 2);
}

uint8_t mpu6050_is_data_ready(void)
{
    // Interrupt Status, pg. 28
    // Reading this clear the interrupt bits when INT_RD_CLEAR bit in INT_PIN_CFG is 0
    uint8_t int_status[2] = {INT_STATUS, 0x00};
    (*i2c_read)(0x68, int_status, 2);
    return int_status[1] & 0x01;
}

void mpu6050_read_data(mpu6050_data* store)
{
    // TODO: Improve I2C api
    uint8_t data[sizeof(*store)];
    uint8_t reg_addr[2] = {ACCEL_XOUT_H, 0x00};
    uint8_t* p = data;
    while (reg_addr[0] <= GYRO_ZOUT_L) {
        i2c_status_e result = (*i2c_read)(0x68, &reg_addr[0], 2);
        if (result == I2C_STATUS_OK) {
            *p++ = reg_addr[1];
            reg_addr[0]++;
        }
    }

    // parse raw data
    store->accx = (data[0] << 8) | data[1];
    store->accy = (data[2] << 8) | data[3];
    store->accz = (data[4] << 8) | data[5];
    store->temp = (data[6] << 8) | data[7];
    store->gyrox = (data[8] << 8) | data[9];
    store->gyroy = (data[10] << 8) | data[11];
    store->gyroz = (data[12] << 8) | data[13];

    // printf_("AccelX: %d, AccelY: %d, AccelZ: %d \n\r", store->accx, store->accy, store->accz);
    // printf_("Temp: %d", store->temp)
    // printf_("GyroX: %lu, GyroY: %lu \n\r", store->gyrox, store->gyroy);
}