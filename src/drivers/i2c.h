#pragma once

#include <stdint.h>

#define I2C_100Kbps 100000U
#define I2C_400Kbps 400000U
#define I2C_1Mbps 1000000U

// Used I2C Module 1 at GPIOA
#define SCL_PIN (1U << 6)
#define SDA_PIN (1U << 7)

typedef enum i2c_status_e {
    I2C_STATUS_OK = 0,
    I2C_STATUS_BUSY = (1U << 0),
    I2C_STATUS_ERROR = (1U << 1),
    I2C_STATUS_ADRACK = (1U << 2),
    I2C_STATUS_DATACK = (1U << 3),
    I2C_STATUS_ARBLST = (1U << 4),
    I2C_STATUS_IDLE = (1U << 5),
    I2C_STATUS_BUSBSY = (1U << 6),
    I2C_STATUS_CLKTO = (1U << 7)
} i2c_status_e;

typedef enum { I2C_MASTER_TRANSMIT, I2C_MASTER_RECEIVE } master_mode;
typedef enum {
    I2C_START_RUN_OP = (1U << 1 | 1U << 0),
    I2C_STOP_START_RUN_OP = (1U << 2 | 1U << 1 | 1U << 0),
    I2C_RUN_OP = (1U << 0),
    I2C_STOP_RUN_OP = (1U << 2 | 1U << 0)
} master_operation;

typedef i2c_status_e (*i2c_write_handler)(uint8_t slave_addr, uint8_t* buffer, uint8_t buf_size);
typedef i2c_status_e (*i2c_read_handler)(uint8_t slave_addr, uint8_t reg_addr, uint8_t* store,
                                         uint8_t store_size);

void i2c1_init(uint32_t i2c_speed);
void i2c1_wait_tx_rx();
void i2c1_set_slave_address(uint8_t addr, master_mode mode);
void i2c1_transmit_byte(uint8_t byte, master_operation op);
void i2c1_generate_stop(void);
i2c_status_e i2c1_write(uint8_t slave_addr, uint8_t* buffer, uint8_t buf_size);
i2c_status_e i2c1_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t* store, uint8_t store_size);
