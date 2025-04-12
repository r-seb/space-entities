#include "active_object.h"
#include "app.h"
#include "printf.h"

typedef struct {
    Active super;

    // private:
} I2CManager;

static I2CManager i2c_manager;
Active* AO_I2CManager = &i2c_manager.super; // Global pointer so that others can post events to it

// Function prototypes
static void I2CManager_ctor(I2CManager* const me);
static void I2CManager_dispatch(I2CManager* me, Event const* const e);

void I2CManager_ctor_call() { I2CManager_ctor(&i2c_manager); } // To be called by main

static void I2CManager_ctor(I2CManager* const me)
{
    Active_ctor(&me->super, (ao_dispatch_handler)&I2CManager_dispatch);
}

static void I2CManager_dispatch(I2CManager* me, Event const* const e)
{
    switch (e->sig) {
        case INIT_SIG: {
            printf_("I2CManager Initialized\n\r");
            // static Event const mine_img_evt = {MINE_IMG_SIG};
            // Active_post(AO_Passageway, &mine_img_evt);
            PositionEvent* pos_evt;
            EVENT_ALLOCATE(pos_evt);
            pos_evt->super.sig = SHIP_IMG_SIG;
            pos_evt->x = 0xD;
            pos_evt->y = 0xB;
            Active_post(AO_Passageway, (Event*)pos_evt);
        } break;
        default:
            printf_("I2CManager Default State\n\r");
            break;
    }
}
