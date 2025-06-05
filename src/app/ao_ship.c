#include "active_object.h"
#include "app.h"
#include "i2c.h"
#include "mpu6050_i2c.h"
#include "state_machine.h"
#include "uart.h"

typedef struct {
    Active super;

    // private:
    TimeEvent time_event;
    uint8_t imu_read_buffer[14];
    mpu6050_data imu_parsed_data;
    uint8_t position_x;
    uint8_t position_y;
} Ship;

static Ship ship;
Active* AO_Ship = &ship.super; // Global pointer so that others can post events to it

// Function prototypes
static void Ship_ctor(Ship* const me);
static State Ship_initial(Ship* me, Event const* const e);
static State Ship_flying(Ship* me, Event const* const e);
static State Ship_destroyed(Ship* me, Event const* const e);

// To be called by main
void Ship_ctor_call()
{
    Ship_ctor(&ship);
}

static void Ship_ctor(Ship* const me)
{
    Active_ctor(&me->super, (StateHandler)&Ship_initial);
    TimeEvent_ctor(&me->time_event, TIME_OUT_SIG, &me->super);
}

// ---------------------------------------------------------------------------------------------//
// Helper Functions

// ---------------------------------------------------------------------------------------------//
// State Machine
static State Ship_initial(Ship* me, Event const* const e)
{
    uart_send("Ship Initialized\n\r");
    mpu6050_init(&i2c1_write, &i2c1_read);
    return TRAN(&Ship_flying);
}

static uint16_t count = 0;
static State Ship_flying(Ship* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            // Post the TIME_OUT_SIG every 10Hz or 5*0.02ms
            TimeEvent_arm(&me->time_event, 5U, 5U);
            state_stat = HANDLED_STATUS;
        } break;
        case TIME_TICK_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        case TIME_OUT_SIG: {
            mpu6050_get_data(me->imu_read_buffer, AO_Ship);
            state_stat = HANDLED_STATUS;
        } break;
        case I2C_RECEIVE_DONE_SIG: {
            mpu6050_parse_data(me->imu_read_buffer, &me->imu_parsed_data);
            uart_send("%d\n\r", count++);
            uart_send("Accx: %f\n\rAccy: %f\n\rAccz: %f\n\r", me->imu_parsed_data.accx,
                      me->imu_parsed_data.accy, me->imu_parsed_data.accz);
            state_stat = HANDLED_STATUS;
        } break;
        default:
            uart_send("Ship Default State\n\r");
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}

static State Ship_destroyed(Ship* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        case EXIT_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
