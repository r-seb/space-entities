#pragma once

#include "active_object.h"
#include "i2c.h"
#include "stdint.h"

#define UART_MSG_SIZE 100U
#define UART_DEFERRED_QUEUE_SIZE 32U

#define I2C_DEFERRED_QUEUE_SIZE 64U

extern TX_BYTE_POOL msg_evt_byte_pool;
extern TX_BYTE_POOL uart_evt_byte_pool;

enum GameSignals {
    TIME_TICK_SIG = USER_SIG,
    TIME_OUT_SIG,
    SHIP_MOVE_SIG,
    COLLIDED_SIG,

    GAME_MAX_SIG
};

enum UartManagerSignals {
    UART_SEND_SIG = GAME_MAX_SIG,
    UART_TRANSMITTED_SIG,

    UART_MAX_SIG
};

enum I2CManagerSignals {
    I2C_TRANSMIT_START_SIG = UART_MAX_SIG,
    I2C_RECEIVE_START_SIG,
    I2C_TRANSACTION_OK_SIG,
    I2C_ERROR_SIG,
    I2C_RECEIVE_DONE_SIG,
    I2C_START_BUS_RECOVERY_SIG,
    I2C_TOGGLE_SCL_SIG,
    I2C_CHECK_SDA_SIG,

    I2C_MAX_SIG
};

// ---------------------------------------------------------------------------------------------//
// Event with parameters
typedef struct {
    Event super;

    // public:
    uint8_t move_cmd; // BITMASK. 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT
} PositionEvent;

typedef struct {
    Event super;

    // public:
    uint8_t buffer_size;
    char buffer[];
} SerialEvent;

typedef struct {
    Event super;

    // public:
    i2c_data data;
} I2CEvent;

// ---------------------------------------------------------------------------------------------//
// Active objects
void Passageway_ctor_call(void);
extern Active* AO_Passageway; // Global pointer so that others can post events to it

void I2CManager_ctor_call(void);
extern Active* AO_I2CManager; // Global pointer so that others can post events to it

void UARTManager_ctor_call(void);
extern Active* AO_UARTManager; // Global pointer so that others can post events to it

void Ship_ctor_call(void);
extern Active* AO_Ship; // Global pointer so that others can post events to it
