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
    uint16_t bus_recovery_retries;
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
static State I2CManager_bus_recovery(I2CManager* me, Event const* const e);

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
    me->bus_recovery_retries = 0;
    i2c1_init(I2C_1Mbps);
}

static void I2CManager_set_cur_data(Event const* const e)
{
    i2c_manager.cur_evt = (Event*)e;
    i2c_manager.cur_i2c_data = (i2c_data*)&EVENT_CAST(e, I2CEvent)->data;
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
                me->deferred_queue.events_loss = 0;
            }

            // Check bus status, we have to make sure the bus is not hang
            if (i2c1_check_status(I2C_STATUS_BUSBSY)) {
                uart_send("I2C bus hang\n\r");
                static const Event bus_recovery_evt = {I2C_START_BUS_RECOVERY_SIG};
                Active_post_front(AO_I2CManager, &bus_recovery_evt);
            } else {
                // Do we have deferred i2c events to process?
                Event* evt_addr = (Event*)0;
                if (Queue_pop(&me->deferred_queue, &evt_addr) == QUEUE_SUCCESS) {
                    Active_post_front(AO_I2CManager, evt_addr);
                }
            }
            state_stat = HANDLED_STATUS;
        } break;
        case I2C_ERROR_SIG: {
            uart_send("I2C error in Idle\n\r");
            state_stat = TRAN(&I2CManager_idle);
        } break;
        case I2C_START_BUS_RECOVERY_SIG: {
            uart_send("I2C attempting bus recovery\n\r");
            state_stat = TRAN(&I2CManager_bus_recovery);
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

// https://e2e.ti.com/support/microcontrollers/arm-based-microcontrollers-group/arm-based-microcontrollers/f/arm-based-microcontrollers-forum/954162/tm4c1294ncpdt-problems-resetting-i2c-communication
/* The bus can sometimes hang due to a variety of reasons. It can be due to noise generating extra
 * transitions on SCL or SDA causing the master/slave out of synchronisation or a slave misbehaving.
 * When this happens, the MCU was not able to do any I2C operation on that particular bus. Even
 * resetting the board is sometimes futile and only a power cycle can release the bus from being
 * locked.
 *
 * The solution proposed from the above link is by re-configuring the I2C SCL and SDA pins as a GPIO
 * and bit-banging or toggling the SCL pin high/low until the SDA line is released. This state does
 * exactly that. Once the bus is free, this state will re-configure the GPIO back to I2C.
 */
static State I2CManager_bus_recovery(I2CManager* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            i2c1_init_bus_recovery();
            state_stat = HANDLED_STATUS;
            static const Event toggle_scl_evt = {I2C_TOGGLE_SCL_SIG};
            Active_post(AO_I2CManager, &toggle_scl_evt);
        } break;
        case I2C_CHECK_SDA_SIG: {
            me->bus_recovery_retries++;
            // Check if bus is still locked, SDA high means it's free.
            if (i2c1_check_sda()) {
                // Make sure to leave SCL high
                i2c1_set_scl_high();

                uart_send("I2C bus recovery retries: %d\n\r", me->bus_recovery_retries);
                me->bus_recovery_retries = 0;

                state_stat = TRAN(&I2CManager_idle);
            }
            // Otherwise, we'll continue the recovery process
            else {
                static const Event toggle_scl_evt = {I2C_TOGGLE_SCL_SIG};
                Active_post(AO_I2CManager, &toggle_scl_evt);
                state_stat = HANDLED_STATUS;
            }
        } break;
        case I2C_TOGGLE_SCL_SIG: {
            i2c1_toggle_scl();
            static const Event check_sda_evt = {I2C_CHECK_SDA_SIG};
            Active_post(AO_I2CManager, &check_sda_evt);
            state_stat = HANDLED_STATUS;
        } break;
        // TODO: Implement HSM to capture common behaviour in a Busy state
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
        case EXIT_SIG: {
            // We re-initialize back to I2C
            i2c1_init(I2C_1Mbps);
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
