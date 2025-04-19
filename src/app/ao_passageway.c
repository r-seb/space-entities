#include "active_object.h"
#include "app.h"
#include "led.h"
#include "printf.h"

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
static void Passageway_dispatch(Passageway* me, Event const* const e);

void Passageway_ctor_call() { Passageway_ctor(&passageway); } // To be called by main

static void Passageway_ctor(Passageway* const me)
{
    Active_ctor(&me->super, (StateHandler)&Passageway_dispatch);
    TimeEvent_ctor(&me->time_event, TIME_OUT_SIG, &me->super);
}

static void Passageway_dispatch(Passageway* me, Event const* const e)
{
    UINT status;
    switch (e->sig) {
        case INIT_SIG: {
            printf_("Passageway Initialized\n\r");
            // Post the TIME_OUT_SIG every 1 second
            TimeEvent_arm(&me->time_event, 1000U, 1000U);
        } break;
        case TIME_OUT_SIG: {
            led_toggle(LED_BLUE);
        } break;
        case SHIP_IMG_SIG: {
            printf_("Mine PosX=%u, PosY=%u \n\r", EVENT_CAST(PositionEvent)->x,
                    EVENT_CAST(PositionEvent)->y);
            status = EVENT_HANDLED();
            printf_("Byte Released: %d\n\r", status);
        } break;
        default:
            printf_("Passageway Default State\n\r");
            break;
    }
}
