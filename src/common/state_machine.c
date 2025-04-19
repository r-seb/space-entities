#include "state_machine.h"
#include "assert_handler.h"

static Event const entryEvt = {ENTRY_SIG};
static Event const exitEvt = {EXIT_SIG};

void Hsm_ctor(Hsm* const me, StateHandler initial) { me->state = initial; }

void Hsm_init(Hsm* const me, Event const* const e)
{
    ASSERT(me->state != (StateHandler)0);
    (*me->state)(me, e);
    (*me->state)(me, &entryEvt);
}

void Hsm_dispatch(Hsm* const me, Event const* const e)
{
    State status;
    StateHandler prev_state = me->state; // save for later

    ASSERT(me->state != (StateHandler)0);
    status = (*me->state)(me, e);

    if (status == TRAN_STATUS) {
        (*prev_state)(me, &exitEvt);
        (*me->state)(me, &entryEvt);
    }
}
