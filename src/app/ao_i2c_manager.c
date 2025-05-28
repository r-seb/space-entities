#include "active_object.h"
#include "app.h"
#include "assert_handler.h"
#include "i2c.h"
#include "state_machine.h"
#include "uart.h"
#include <stdbool.h>

// ---------------------------------------------------------------------------------------------//
// Simple FIFO
// Based on: https://www.downtowndougbrown.com/2013/01/microcontrollers-interrupt-safe-ring-buffers/
typedef struct {
    Event* storage[I2C_DEFERRED_QUEUE_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t size; // The array size, must be initialized
} Queue;

typedef enum { QUEUE_SUCCESS, QUEUE_FULL, QUEUE_EMPTY } Queue_status;

static Queue_status Queue_push(Queue* q, Event const* const e)
{
    ASSERT(q && e);

    uint16_t next_head = (q->head + 1) % q->size;
    if (next_head != q->tail) {
        q->storage[q->head] = (Event*)e;
        q->head = next_head;
        return QUEUE_SUCCESS;
    }
    return QUEUE_FULL;
}

static Queue_status Queue_pop(Queue* q, Event** store)
{
    ASSERT(q && store);

    if (q->tail != q->head) {
        *store = q->storage[q->tail];
        q->tail = (q->tail + 1) % q->size;
        return QUEUE_SUCCESS;
    }
    return QUEUE_EMPTY;
}

// ---------------------------------------------------------------------------------------------//
// Active object
typedef struct {
    Active super;

    // private:
    Queue deferred_queue;
    i2c_data cur_data;
} I2CManager;

static I2CManager i2c_manager;
Active* AO_I2CManager = &i2c_manager.super; // Global pointer so that others can post events to it

// Function prototypes
static void I2CManager_ctor(I2CManager* const me);
static void I2CManager_set_cur_buffer(Event const* const e, bool transmit);
static State I2CManager_initial(I2CManager* me, Event const* const e);
static State I2CManager_idle(I2CManager* me, Event const* const e);
static State I2CManager_master_transmit(I2CManager* me, Event const* const e);
static State I2CManager_master_receive(I2CManager* me, Event const* const e);

// To be called by main
void I2CManager_ctor_call()
{
    I2CManager_ctor(&i2c_manager);
}

static void I2CManager_ctor(I2CManager* const me)
{
    Active_ctor(&me->super, (StateHandler)&I2CManager_initial);
    me->deferred_queue.head = 0;
    me->deferred_queue.tail = 0;
    me->deferred_queue.size = I2C_DEFERRED_QUEUE_SIZE;
    i2c1_init(I2C_1Mbps);
}

static void I2CManager_set_cur_buffer(Event const* const e, bool transmit)
{
    i2c_manager.cur_data.slave_addr = EVENT_CAST(e, I2CEvent)->data.slave_addr;
    i2c_manager.cur_data.read_addr = EVENT_CAST(e, I2CEvent)->data.read_addr;
    i2c_manager.cur_data.buffer_size = EVENT_CAST(e, I2CEvent)->data.buffer_size;
    if (transmit) {
        memcpy(i2c_manager.cur_data.write_buffer, EVENT_CAST(e, I2CEvent)->data.write_buffer,
               i2c_manager.cur_data.buffer_size);
    } else {
        i2c_manager.cur_data.store_buffer = EVENT_CAST(e, I2CEvent)->data.store_buffer;
    }

    EVENT_HANDLED(e);
}

// ---------------------------------------------------------------------------------------------//
// State Machine
// Based on Table 16-5. Write Field Decoding for I2CMCS[3:0] Field, pg. 1023
static State I2CManager_initial(I2CManager* me, Event const* const e)
{
    return TRAN(&I2CManager_idle);
}

static State I2CManager_idle(I2CManager* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            // Do we have deferred i2c events to process?
            Event* evt_addr = (Event*)0;
            if (Queue_pop(&me->deferred_queue, &evt_addr) == QUEUE_SUCCESS) {
                Active_post_front(AO_I2CManager, evt_addr);
            }
            state_stat = HANDLED_STATUS;
        } break;
        case I2C_TRANSMIT_START_SIG: {
            I2CManager_set_cur_buffer(e, true);
            state_stat = TRAN(&I2CManager_master_transmit);
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}

// refer Figure 16-8. Master Single TRANSMIT, pg. 1008
// refer Figure 16-10. Master TRANSMIT of Multiple Data Bytes, pg. 1010
static State I2CManager_master_transmit(I2CManager* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            me->cur_data.mode = I2C_MASTER_TRANSMIT;
            i2c1_transmit(&me->cur_data);
            state_stat = HANDLED_STATUS;
        } break;
        case I2C_ERROR_SIG: {
            uart_send("I2CManager error occured\n\r");
            i2c1_generate_stop();
            state_stat = TRAN(&I2CManager_idle);
        } break;
        case I2C_TRANSACTION_OK_SIG: {
            state_stat = TRAN(&I2CManager_idle);
        } break;
        case I2C_TRANSMIT_START_SIG: {
            //  Defer the event, it will be handled back in idle state
            Queue_push(&me->deferred_queue, e);
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}

static State I2CManager_master_receive(I2CManager* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
