#include "active_object.h"
#include "assert_handler.h"
#include "printf.h"
#include "tx_api.h"
#include <stdint.h>

void Active_ctor(Active* const me, ao_dispatch_handler dispatch) { me->dispatch = dispatch; }

// Thread function for all Active Objects
void Active_event_loop(ULONG entry_input)
{
    // NOTE: the tx_thread_create function uses `ULONG` as entry_input, so we'll need to cast it.
    // ref1: https://github.com/eclipse-threadx/threadx/issues/413
    // ref2: https://github.com/eclipse-threadx/threadx/pull/227
    Active* me = (Active*)entry_input; /* the AO instance "me" */

    // Initialize the Active Object
    static Event const init_evt = {INIT_SIG};
    (*me->dispatch)(me, &init_evt);

    // Message Pump / Event Loop
    while (1) {
        Event const* e; // pointer to event object ("message")
        UINT status = tx_queue_receive(&me->queue, (VOID*)&e, TX_WAIT_FOREVER);
        ASSERT(status == TX_SUCCESS);

        // dispatch event to the active object "me"
        (*me->dispatch)(me, e);
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
