#include "active_object.h"
#include "app.h"
#include "game/game.h"
#include "i2c.h"
#include "led.h"
#include "ssd1309_128x64_i2c.h"
#include "state_machine.h"
#include "u8g2.h"
#include "uart.h"

static u8g2_t oled;

typedef struct {
    Active super;

    // private:
    TimeEvent time_event;
} Passageway;

static Passageway passageway;
Active* AO_Passageway = &passageway.super; // Global pointer so that others can post events to it

// Function prototypes
static void Passageway_ctor(Passageway* const me);
static State Passageway_initial(Passageway* me, Event const* const e);
static State Passageway_playing(Passageway* me, Event const* const e);
static State Passageway_game_over(Passageway* me, Event const* const e);

void Passageway_ctor_call()
{
    Passageway_ctor(&passageway);
} // To be called by main

static void Passageway_ctor(Passageway* const me)
{
    Active_ctor(&me->super, (StateHandler)&Passageway_initial);
    TimeEvent_ctor(&me->time_event, TIME_OUT_SIG, &me->super);
}

// ---------------------------------------------------------------------------------------------//
// State Machine
static State Passageway_initial(Passageway* me, Event const* const e)
{
    uart_send("Passageway Initialized\n\r");
    ssd1309_128x64_init(&oled, &i2c1_write);
    game_init();
    return TRAN(&Passageway_playing);
}

static bool init = false;
static State Passageway_playing(Passageway* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            // Post the TIME_OUT_SIG every 1 second
            TimeEvent_arm(&me->time_event, 62U, 62U);
            state_stat = HANDLED_STATUS;
        } break;
        case TIME_TICK_SIG: {
            if (init) {
                game_system_move();
                game_system_draw(&oled);
                game_system_animate();
            }
            state_stat = HANDLED_STATUS;
        } break;
        case TIME_OUT_SIG: {
            led_toggle(LED_BLUE);
            init = true;
            state_stat = HANDLED_STATUS;
        } break;
        case SHIP_MOVE_SIG: {
            game_system_input(e);
            EVENT_HANDLED(e);
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
