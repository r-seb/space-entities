#include "active_object.h"
#include "app.h"
#include "i2c.h"
#include "led.h"
#include "ssd1309_128x64_i2c.h"
#include "state_machine.h"
#include "uart.h"

typedef struct {
    Active super;

    // private:
    TimeEvent time_event;
    uint8_t* mines[5];
    uint8_t last_mine_x;
    uint8_t last_mine_y;
} Passageway;

static Passageway passageway;
Active* AO_Passageway = &passageway.super; // Global pointer so that others can post events to it

// Function prototypes
static void Passageway_ctor(Passageway* const me);
static State Passageway_initial(Passageway* me, Event const* const e);
static State Passageway_playing(Passageway* me, Event const* const e);
static State Passageway_game_over(Passageway* me, Event const* const e);

void Passageway_ctor_call() { Passageway_ctor(&passageway); } // To be called by main

static void Passageway_ctor(Passageway* const me)
{
    Active_ctor(&me->super, (StateHandler)&Passageway_initial);
    TimeEvent_ctor(&me->time_event, TIME_OUT_SIG, &me->super);
}

// ---------------------------------------------------------------------------------------------//
// State Machine
static State Passageway_initial(Passageway* me, Event const* const e)
{
    return TRAN(&Passageway_playing);
}

static State Passageway_playing(Passageway* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            uart_send("Passageway Initialized\n\r");
            // Post the TIME_OUT_SIG every 1 second
            TimeEvent_arm(&me->time_event, 25U, 25U);
            state_stat = HANDLED_STATUS;
        } break;
        case TIME_TICK_SIG: {
            //uart_send("TICK-TOCK");
            state_stat = HANDLED_STATUS;
            Passageway_move(me);
        } break;
        case TIME_OUT_SIG: {
            led_toggle(LED_BLUE);
            state_stat = HANDLED_STATUS;
        } break;
        case SHIP_IMG_SIG: {
            uart_send("Mine PosX=%u, PosY=%u \n\r", EVENT_CAST(PositionEvent)->x,
                      EVENT_CAST(PositionEvent)->y);
            EVENT_HANDLED(e);
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
