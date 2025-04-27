#pragma once

#include "active_object.h"
#include "stdint.h"

#define UART_MSG_SIZE 41U
#define UART_DEFERRED_QUEUE_SIZE 32U

enum GameSignals {
    TIME_TICK_SIG = USER_SIG,
    TIME_OUT_SIG,
    MINE_IMG_SIG,
    SHIP_IMG_SIG,

    PLAYER_SHIP_UP_SIG,
    PLAYER_SHIP_DOWN_SIG,
    PLAYER_SHIP_LEFT_SIG,
    PLAYER_SHIP_RIGHT_SIG,

    GAME_MAX_SIG
};

enum UartManagerSignals {
    UART_SEND_SIG = GAME_MAX_SIG,
    UART_PROCESS_DEFERRED_SIG,
    UART_TRANSMITTED_SIG,

    UART_MAX_SIG
};

// ---------------------------------------------------------------------------------------------//
// Event with parameters
typedef struct {
    Event super;

    // public:
    uint8_t x;
    uint8_t y;
} PositionEvent;

typedef struct {
    Event super;

    // public:
    char buffer[UART_MSG_SIZE];
} SerialEvent;

// ---------------------------------------------------------------------------------------------//
// Active objects
void Passageway_ctor_call(void);
extern Active* AO_Passageway; // Global pointer so that others can post events to it

void I2CManager_ctor_call(void);
extern Active* AO_I2CManager; // Global pointer so that others can post events to it

void UARTManager_ctor_call(void);
extern Active* AO_UARTManager; // Global pointer so that others can post events to it
