#include "active_object.h"
#include "app.h"
#include "assert_handler.h"
#include "state_machine.h"
#include "uart.h"
#include <stdint.h>
#include <string.h>

// ---------------------------------------------------------------------------------------------//
// Simple FIFO
// Based on: https://www.downtowndougbrown.com/2013/01/microcontrollers-interrupt-safe-ring-buffers/
typedef struct {
    Event* storage[UART_DEFERRED_QUEUE_SIZE];
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
    char cur_buffer[UART_MSG_SIZE];
    char* char_ptr;
    Queue deferred_queue;
} UARTManager;

static UARTManager uart_manager;
Active* AO_UARTManager = &uart_manager.super; // Global pointer so that others can post events to it

// Function prototypes
static void UARTManager_ctor(UARTManager* const me);
static void UARTManager_set_cur_buffer(Event const* const e);
static State UARTManager_initial(UARTManager* me, Event const* const e);
static State UARTManager_idle(UARTManager* me, Event const* const e);
static State UARTManager_busy(UARTManager* me, Event const* const e);

void UARTManager_ctor_call() { UARTManager_ctor(&uart_manager); } // To be called by main

static void UARTManager_ctor(UARTManager* const me)
{
    Active_ctor(&me->super, (StateHandler)&UARTManager_initial);
    me->deferred_queue.head = 0;
    me->deferred_queue.tail = 0;
    me->deferred_queue.size = UART_DEFERRED_QUEUE_SIZE;
    uart_init(UART_115200);
}

static void UARTManager_set_cur_buffer(Event const* const e)
{
    // TODO: Experiment with DMA
    memcpy(uart_manager.cur_buffer, EVENT_CAST(SerialEvent)->buffer, UART_MSG_SIZE);
    uart_manager.char_ptr = uart_manager.cur_buffer;
    EVENT_HANDLED(e);
}

// ---------------------------------------------------------------------------------------------//
// State Machine
static State UARTManager_initial(UARTManager* me, Event const* const e)
{
    return TRAN(&UARTManager_idle);
}

static State UARTManager_idle(UARTManager* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            // Do we have deferred serial events to process?
            Event* evt_addr = (Event*)0;
            if (Queue_pop(&me->deferred_queue, &evt_addr) == QUEUE_SUCCESS) {
                UARTManager_set_cur_buffer(evt_addr);
                static Event const process_deferred_sig = {UART_PROCESS_DEFERRED_SIG};
                Active_post_front(AO_UARTManager, &process_deferred_sig);
            }
            state_stat = HANDLED_STATUS;
        } break;
        case UART_PROCESS_DEFERRED_SIG: {
            state_stat = TRAN(&UARTManager_busy);
        } break;
        case UART_SEND_SIG: {
            UARTManager_set_cur_buffer(e);
            state_stat = TRAN(&UARTManager_busy);
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}

static State UARTManager_busy(UARTManager* me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            uart_fill_fifo(&me->char_ptr);
            state_stat = HANDLED_STATUS;
        } break;
        case UART_TRANSMITTED_SIG: {
            // Are we finished?
            if (*me->char_ptr == '\0') {
                state_stat = TRAN(&UARTManager_idle);
            }
            // Transfer the remaining bytes
            else {
                uart_fill_fifo(&me->char_ptr);
                state_stat = HANDLED_STATUS;
            }
        } break;
        case UART_SEND_SIG: {
            // Defer the serial event, it will be handled back in idle state
            Queue_push(&me->deferred_queue, e);
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
