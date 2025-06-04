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
    uint16_t events_loss;
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
    Event* cur_evt;
    i2c_data* cur_i2c_data;
} I2CManager;

static I2CManager i2c_manager;
Active* AO_I2CManager = &i2c_manager.super; // Global pointer so that others can post events to it

// Function prototypes
static void I2CManager_ctor(I2CManager* const me);
static void I2CManager_set_cur_data(Event const* const e);
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

static void I2CManager_set_cur_data(Event const* const e)
{
    i2c_manager.cur_evt = (Event*)e;
    i2c_manager.cur_i2c_data = (i2c_data*)&EVENT_CAST(e, I2CEvent)->data;
    i2c_manager.deferred_queue.events_loss = 0;
}

// ---------------------------------------------------------------------------------------------//
// State Machine
// Based on Table 16-5. Write Field Decoding for I2CMCS[3:0] Field, pg. 1023
static State I2CManager_initial(I2CManager* me, Event const* const e)
{
    uart_send("I2CManager Initialized\n\r");
    return TRAN(&I2CManager_idle);
}

static State I2CManager_idle(I2CManager* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            // Report missed events count
            if (me->deferred_queue.events_loss) {
                uart_send("I2C events loss: %d\n\r", me->deferred_queue.events_loss);
            }
            // Do we have deferred i2c events to process?
            Event* evt_addr = (Event*)0;
            if (Queue_pop(&me->deferred_queue, &evt_addr) == QUEUE_SUCCESS) {
                Active_post_front(AO_I2CManager, evt_addr);
            }
            state_stat = HANDLED_STATUS;
        } break;
        case I2C_ERROR_SIG: {
            uart_send("I2C error in Idle\n\r");
            state_stat = TRAN(&I2CManager_idle);
        } break;
        case I2C_RECEIVE_START_SIG: {
            I2CManager_set_cur_data(e);
            state_stat = TRAN(&I2CManager_master_receive);
        } break;
        case I2C_TRANSMIT_START_SIG: {
            I2CManager_set_cur_data(e);
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
            me->cur_i2c_data->mode = I2C_MASTER_TRANSMIT;
            i2c1_transmit(me->cur_i2c_data);
            state_stat = HANDLED_STATUS;
        } break;
        // TODO: Implement HSM to capture common behaviour in a Busy state
        case I2C_TRANSACTION_OK_SIG: {
            EVENT_HANDLED(me->cur_evt);
            state_stat = TRAN(&I2CManager_idle);
        } break;
        case I2C_ERROR_SIG: {
            uart_send("I2C error in Transmit\n\r");
            i2c1_generate_stop();
            EVENT_HANDLED(me->cur_evt);
            state_stat = TRAN(&I2CManager_idle);
        } break;
        case I2C_RECEIVE_START_SIG: // Intentionally fall through
        case I2C_TRANSMIT_START_SIG: {
            //  Defer the event, it will be handled back in idle state
            Queue_status qstat = Queue_push(&me->deferred_queue, e);
            if (qstat == QUEUE_FULL) {
                // The queue is full, so we just release the event
                EVENT_HANDLED(e);
                (me->deferred_queue.events_loss)++;
            }
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}

// refer pg. 1009, Figure 16-9. Master Single RECEIVE
// refer pg. 1011, Figure 16-11. Master RECEIVE of Multiple Data Bytes
static State I2CManager_master_receive(I2CManager* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            // Transmit read address
            me->cur_i2c_data->mode = I2C_MASTER_TRANSMIT;
            i2c1_transmit(me->cur_i2c_data);
            state_stat = HANDLED_STATUS;
        } break;
        // TODO: Implement HSM to capture common behaviour in a Busy state
        case I2C_TRANSACTION_OK_SIG: {
            if (me->cur_i2c_data->mode == I2C_MASTER_TRANSMIT) {
                // Transmit read address was a success so we start receiving
                me->cur_i2c_data->mode = I2C_MASTER_RECEIVE;
                i2c1_receive(me->cur_i2c_data);
                state_stat = HANDLED_STATUS;
            } else {
                // Check if the requester needs to be notified
                static const Event done_evt = {I2C_RECEIVE_DONE_SIG};
                if (me->cur_i2c_data->requester != NULL) {
                    Active_post_front(me->cur_i2c_data->requester, &done_evt);
                }
                EVENT_HANDLED(me->cur_evt);
                state_stat = TRAN(&I2CManager_idle);
            }
        } break;
        case I2C_ERROR_SIG: {
            uart_send("I2C error in Receive\n\r");
            i2c1_generate_stop();
            EVENT_HANDLED(me->cur_evt);
            state_stat = TRAN(&I2CManager_idle);
        } break;
        case I2C_RECEIVE_START_SIG: // Intentionally fall through
        case I2C_TRANSMIT_START_SIG: {
            //  Defer the event, it will be handled back in idle state
            Queue_status qstat = Queue_push(&me->deferred_queue, e);
            if (qstat == QUEUE_FULL) {
                // The queue is full, so we just release the event
                EVENT_HANDLED(e);
                (me->deferred_queue.events_loss)++;
            }
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
