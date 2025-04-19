#include "active_object.h"
#include "assert_handler.h"
#include "printf.h"
#include "tx_api.h"
#include <stdint.h>

// ---------------------------------------------------------------------------------------------//
// Active Object Facilities
void Active_ctor(Active* const me, StateHandler initial) { Hsm_ctor(&me->super, initial); }

// Thread function for all Active Objects
void Active_event_loop(ULONG entry_input)
{
    // NOTE: the tx_thread_create function uses `ULONG` as entry_input, so we'll need to cast it.
    // ref1: https://github.com/eclipse-threadx/threadx/issues/413
    // ref2: https://github.com/eclipse-threadx/threadx/pull/227
    Active* me = (Active*)entry_input; /* the AO instance "me" */

    // Initialize the Active Object
    static Event const init_evt = {INIT_SIG};
    Hsm_init(&me->super, &init_evt);

    // Message Pump / Event Loop
    while (1) {
        Event const* e; // pointer to event object ("message")
        UINT status = tx_queue_receive(&me->queue, (VOID*)&e, TX_WAIT_FOREVER);
        ASSERT(status == TX_SUCCESS);

        // dispatch event to the active object "me"
        Hsm_dispatch(&me->super, e);
    }
}

void Active_start(Active* const me, UINT prio, TX_BLOCK_POOL* block_pool, uint32_t stack_size,
                  CHAR** stack_ptr, TX_BYTE_POOL* byte_pool, uint32_t queue_len, CHAR** queue_ptr)
{
    ASSERT(me); // AO instance cannot be NULL

    UINT status;

    // Allocate stack and create thread
    tx_block_allocate(block_pool, (VOID**)stack_ptr, TX_NO_WAIT);
    status = tx_thread_create(&me->thread, "private thread", &Active_event_loop, (ULONG)me,
                              *stack_ptr, stack_size, prio, prio, TX_NO_TIME_SLICE, TX_AUTO_START);
    ASSERT(status == TX_SUCCESS);

    // Allocate queue buffer and create queue
    tx_byte_allocate(byte_pool, (VOID**)queue_ptr, queue_len, TX_NO_WAIT);
    status = tx_queue_create(&me->queue, "message queue", TX_1_ULONG, *queue_ptr, queue_len);
    ASSERT(status == TX_SUCCESS);
}

void Active_post(Active* const me, Event const* const e)
{
    UINT status = tx_queue_send(&me->queue, (void*)&e, TX_WAIT_FOREVER);
    ASSERT(status == TX_SUCCESS);
}

void Active_post_nonthread(Active* const me, Event const* const e)
{
    // TX_NO_WAIT is the only valid option for non-thread calls
    // https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/threadx/chapter4.md#parameters-42
    UINT status = tx_queue_send(&me->queue, (void*)&e, TX_NO_WAIT);
    ASSERT(status == TX_SUCCESS);
}

// ---------------------------------------------------------------------------------------------//
// Timer Event Facilities
static TimeEvent* l_time_event_list[10]; // all TimeEvents in the application
static uint8_t l_time_event_num;         // current number of TimeEvents

/*..........................................................................*/
void TimeEvent_ctor(TimeEvent* const me, Signal sig, Active* act)
{
    /* no critical section because it is presumed that all TimeEvents
     * are created *before* multitasking has started.
     */
    me->super.sig = sig;
    me->active_object = act;
    me->timeout = 0U;
    me->interval = 0U;

    /* register one more TimeEvent instance */
    UINT old_posture;
    old_posture = tx_interrupt_control(TX_INT_DISABLE);
    ASSERT(l_time_event_num < sizeof(l_time_event_list) / sizeof(l_time_event_list[0]));
    l_time_event_list[l_time_event_num] = me;
    ++l_time_event_num;
    tx_interrupt_control(old_posture);
}

/*..........................................................................*/
void TimeEvent_arm(TimeEvent* const me, uint32_t timeout, uint32_t interval)
{
    UINT old_posture;
    old_posture = tx_interrupt_control(TX_INT_DISABLE);
    me->timeout = timeout;
    me->interval = interval;
    tx_interrupt_control(old_posture);
}

/*..........................................................................*/
void TimeEvent_disarm(TimeEvent* const me)
{
    UINT old_posture;
    old_posture = tx_interrupt_control(TX_INT_DISABLE);
    me->timeout = 0U;
    tx_interrupt_control(old_posture);
}

/*..........................................................................*/
void TimeEvent_tick(void)
{
    uint_fast8_t i;
    for (i = 0U; i < l_time_event_num; ++i) {
        TimeEvent* const t = l_time_event_list[i];
        ASSERT(t);                    // TimeEvent instance must be registered
        if (t->timeout > 0U) {        // is this TimeEvent armed?
            if (--t->timeout == 0U) { // is it expiring now?
                Active_post_nonthread(t->active_object, &t->super);
                t->timeout = t->interval; // rearm or disarm (one-shot)
            }
        }
    }
}
