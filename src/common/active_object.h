#pragma once

#include "tx_api.h"
#include <stdint.h>

#define EVENT_CAST(type_) ((type_ const*)(e))

#define EVENT_ALLOCATE(pool_, evt_)                                                                \
    (tx_byte_allocate(&(pool_), (VOID**)&(evt_), sizeof(*(evt_)), TX_NO_WAIT))
#define EVENT_HANDLED() (tx_byte_release((VOID*)e))

// ---------------------------------------------------------------------------------------------//
// Event Facilities
typedef uint16_t ao_signal; // event signal

enum ao_reserved_signals {
    INIT_SIG, // dispatched to each AO beforing entering event-loop
    USER_SIG  // first signal available to the users
};

// Event base class
typedef struct {
    ao_signal sig; // event signal

    // event parameters added in subclasses of Event
} Event;

// ---------------------------------------------------------------------------------------------//
// Active Object Facilities
typedef struct Active Active; // forward declaration

typedef void (*ao_dispatch_handler)(Active* const me, Event const* const e);

// Active Object base class
struct Active {
    TX_THREAD thread; // private thread
    TX_QUEUE queue;   // private message queue, provided by user

    ao_dispatch_handler dispatch; // pointer to the dispatch() function

    // active object data added in subclasses of Active
};

void Active_ctor(Active* const me, ao_dispatch_handler dispatch);
void Active_start(Active* const me, UINT prio, TX_BLOCK_POOL* block_pool, uint32_t stack_size,
                  CHAR** stack_ptr, TX_BYTE_POOL* byte_pool, uint32_t queue_len, CHAR** queue_ptr);
void Active_post(Active* const me, Event const* const e);
