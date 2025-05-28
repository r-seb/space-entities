#pragma once

#include "tx_api.h"
#include <stdint.h>

extern TX_BYTE_POOL msg_evt_byte_pool;

#define EVENT_CAST(evt_, type_) ((type_ const*)(evt_))

#define EVENT_ALLOCATE(evt_)                                                                       \
    (tx_byte_allocate(&msg_evt_byte_pool, (VOID**)&(evt_), sizeof(*(evt_)), TX_NO_WAIT))
#define EVENT_ALLOCATE_WITH_SIZE(evt_, size_)                                                      \
    (tx_byte_allocate(&msg_evt_byte_pool, (VOID**)&(evt_), size_, TX_NO_WAIT))
#define EVENT_HANDLED(evt_) (tx_byte_release((VOID*)evt_))

// ---------------------------------------------------------------------------------------------//
// Event Facilities
typedef uint16_t Signal; // event signal

enum reserved_signals {
    INIT_SIG,  // dispatched to each AO beforing entering event-loop
    ENTRY_SIG, // trigger state machine entry action
    EXIT_SIG,  // trigger state machine exit action
    USER_SIG   // first signal available to the users
};

// Event base class
typedef struct {
    Signal sig; // event signal

    // event parameters added in subclasses of Event
} Event;

// ---------------------------------------------------------------------------------------------//
// Hierarchical State Machine Facilities
typedef struct Hsm Hsm; // Forward declare

typedef enum { TRAN_STATUS, HANDLED_STATUS, IGNORED_STATUS, INIT_STATUS } State;

typedef State (*StateHandler)(Hsm* const me, Event const* const e);

#define TRAN(target_) (((Hsm*)me)->state = (StateHandler)(target_), TRAN_STATUS)

struct Hsm {
    StateHandler state; // the State variable
};

// take initial statehandler in the specific
// subclass, such as TimeBomb_initial
void Hsm_ctor(Hsm* const me, StateHandler initial);

// Execute the initial transition and entry action into the first state
void Hsm_init(Hsm* const me, Event const* const e);
// Handle the subsequent events
void Hsm_dispatch(Hsm* const me, Event const* const e);
