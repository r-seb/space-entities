#include "mpu6050_i2c.h"
#include <stdbool.h>
#include <stdint.h>

static i2c_write_handler i2c_write;
static i2c_read_handler i2c_read;

// #define INT_PIN (1U << 5)

// void GPIOPortA_IRQHandler()
// {
//     uint32_t isrs_pins = GPIOA_AHB->MIS;
//     GPIOA_AHB->ICR = 0xFFU; // Clear interrupt sources
//
//     // Only execute this when threadx is running
//     TX_THREAD* thread_ptr;
//     thread_ptr = tx_thread_identify();
//     static const Event evt = {MPU6050_READ_SIG};
//     if ((isrs_pins & INT_PIN) && (thread_ptr != TX_NULL)) {
//         Active_post_nonthread(AO_Ship, &evt);
//     }
// }

void mpu6050_init(i2c_write_handler write_handler, i2c_read_handler read_handler)
{
    i2c_write = write_handler;
    i2c_read = read_handler;

    /* // Assign interrupt pin
    SYSCTL->RCGCGPIO |= (1U << 0);  // GPIOA
    SYSCTL->GPIOHBCTL |= (1U << 0); // Enable High-Performance Bus Control
    GPIOA_AHB->DIR &= ~INT_PIN;     // Configure as input
    GPIOA_AHB->AFSEL &= ~INT_PIN;   // Configure as GPIO
    GPIOA_AHB->PDR |= INT_PIN;      // Pull down register
    GPIOA_AHB->DEN |= INT_PIN;      // Enable GPIO PIN
    // Configure Interrupt, pg. 654
    GPIOA_AHB->IS &= ~INT_PIN;  // Edge detect, pg. 664
    GPIOA_AHB->IBE &= ~INT_PIN; // Only one edge detect interrupt, pg. 665
    GPIOA_AHB->IEV |= INT_PIN;  // A rising edge triggers an interrupt, pg. 666
    GPIOA_AHB->IM |= INT_PIN;   // Enable interrupt, pg. 667
    // Enable interrupt
    NVIC_SetPriority(GPIOA_IRQn, 7);
    NVIC_EnableIRQ(GPIOA_IRQn); */

    // Interrupt pin config, pg. 26, default Active high
    // interrupt status bits are cleared on any read operation
    uint8_t int_pin_cfg[2] = {INT_PIN_CFG, (1U << 4)};
    (*i2c_write)(0x68, int_pin_cfg, 2);

    // Enable interrupt, pg. 27
    uint8_t int_enable[2] = {INT_ENABLE, 0x01};
    (*i2c_write)(0x68, int_enable, 2);

    // Sample Rate Divider, pg. 12
    // With DLPG enabled, Sample Rate = 1000 / (1 + 9) = 100Hz
    uint8_t smplrt_div[2] = {SMPLRT_DIV, 0x09};
    (*i2c_write)(0x68, smplrt_div, 2);

    // Set gyro config, pg. 14, +/- 250 deg/sec
    uint8_t gyro_config[2] = {GYRO_CONFIG, 0x00};
    (*i2c_write)(0x68, gyro_config, 2);

    // Set accel config, pg. 15, +/- 2 g
    uint8_t accel_config[2] = {ACCEL_CONFIG, 0x00};
    (*i2c_write)(0x68, accel_config, 2);

    // Config with DLPG, pg. 13
    uint8_t config[2] = {CONFIG, 0x00};
    (*i2c_write)(0x68, config, 2);

    // Set pwr_mgmt to wake the board, pg. 40
    uint8_t pwr_mgmt_1[2] = {PWR_MGMT_1, 0x01};
    (*i2c_write)(0x68, pwr_mgmt_1, 2);
}

void mpu6050_get_data(uint8_t* data, Active* requester)
{
    (*i2c_read)(0x68, ACCEL_XOUT_H, data, 14, requester);
}

void mpu6050_parse_data(uint8_t const* data, mpu6050_data* const store)
{
    // get raw data
    int16_t accx_raw = (data[0] << 8) | data[1];
    int16_t accy_raw = (data[2] << 8) | data[3];
    int16_t accz_raw = (data[4] << 8) | data[5];
    int16_t temperature_raw = (data[6] << 8) | data[7];
    int16_t gyrox_raw = (data[8] << 8) | data[9];
    int16_t gyroy_raw = (data[10] << 8) | data[11];
    int16_t gyroz_raw = (data[12] << 8) | data[13];

    // parse raw data
    store->accx = (float)accx_raw / 16384;
    store->accy = (float)accy_raw / 16384;
    store->accz = (float)accz_raw / 16384;
    store->temperature = (temperature_raw / 340.0) + 36.43;
    store->gyrox = (float)gyrox_raw / 131;
    store->gyroy = (float)gyroy_raw / 131;
    store->gyroz = (float)gyroz_raw / 131;
}
