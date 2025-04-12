#include "active_object.h"
#include "app.h"
#include "printf.h"

typedef struct {
    Active super;

    // private:
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
    Active_ctor(&me->super, (ao_dispatch_handler)&Passageway_dispatch);
}

static void Passageway_dispatch(Passageway* me, Event const* const e)
{
    UINT status;
    switch (e->sig) {
        case INIT_SIG: {
            printf_("Passageway Initialized\n\r");
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
