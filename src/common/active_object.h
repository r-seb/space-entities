#pragma once

#include "state_machine.h"
#include <stdint.h>

// ---------------------------------------------------------------------------------------------//
// Active Object Facilities
typedef struct Active Active; // forward declaration

// Active Object base class
struct Active {
    Hsm super;
    TX_THREAD thread; // private thread
    TX_QUEUE queue;   // private message queue, provided by user

    // active object data added in subclasses of Active
};

void Active_ctor(Active* const me, StateHandler initial);
void Active_start(Active* const me, UINT prio, TX_BLOCK_POOL* block_pool, uint32_t stack_size,
                  CHAR** stack_ptr, TX_BYTE_POOL* byte_pool, uint32_t queue_len, CHAR** queue_ptr);
void Active_post(Active* const me, Event const* const e);
void Active_post_front(Active* const me, Event const* const e);
void Active_post_nonthread(Active* const me, Event const* const e);

// ---------------------------------------------------------------------------------------------//
// Timer Event Facilities
typedef struct {
    Event super;           // inherit Event
    Active* active_object; // the AO that requested this TimeEvent
    uint32_t timeout;      // timeout counter, 0 means not armed
    uint32_t interval;     // interval for periodic TimeEvent, 0 means one-shot
} TimeEvent;

void TimeEvent_ctor(TimeEvent* const me, Signal sig, Active* act);
void TimeEvent_arm(TimeEvent* const me, uint32_t timeout, uint32_t interval);
void TimeEvent_disarm(TimeEvent* const me);

// static (i.e., class-wide) operation
void TimeEvent_tick(void);
