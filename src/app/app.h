#pragma once

#include "active_object.h"
#include "stdint.h"

enum GameSignals {
    TIME_TICK_SIG = USER_SIG,
    MINE_IMG_SIG,
    SHIP_IMG_SIG,

    PLAYER_SHIP_UP_SIG,
    PLAYER_SHIP_DOWN_SIG,
    PLAYER_SHIP_LEFT_SIG,
    PLAYER_SHIP_RIGHT_SIG,

    MAX_SIG
};

// ---------------------------------------------------------------------------------------------//
// Event with parameters
typedef struct {
    Event super;

    // public:
    uint8_t x;
    uint8_t y;
} PositionEvent;

// ---------------------------------------------------------------------------------------------//
// Active objects
void Passageway_ctor_call(void);
extern Active* AO_Passageway; // Global pointer so that others can post events to it

void I2CManager_ctor_call(void);
extern Active* AO_I2CManager; // Global pointer so that others can post events to it
