#include "app.h"
#include "game/game.h"
#include "state_machine.h"

// Function prototypes
static void sm_player_ctor(state_comp_t* const me);
static State sm_player_initial(state_comp_t* const me, Event const* const e);
static State sm_player_flying(state_comp_t* const me, Event const* const e);
static State sm_player_exploding(state_comp_t* const me, Event const* const e);

// To be called by main
void sm_player_ctor_call(state_comp_t* sm_instance)
{
    sm_player_ctor(sm_instance);
}

static void sm_player_ctor(state_comp_t* const me)
{
    Hsm_ctor(&me->super, (StateHandler)&sm_player_initial);
}

static State sm_player_initial(state_comp_t* me, Event const* const e)
{
    return TRAN(&sm_player_flying);
}

static State sm_player_flying(state_comp_t* const me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        case SHIP_MOVE_SIG: {
            uint8_t cmd = EVENT_CAST(e, PositionEvent)->move_cmd;
            velocity_comp_t* vel =
                ECS_GET_COMP_FROM_ENT(me->ecs, VELOCITY_COMP_ID, me->entity, velocity_comp_t);
            // UP
            if (cmd & (1U << 0)) {
                vel->dy = -1.f;
            }
            // DOWN
            else if (cmd & (1U << 1)) {
                vel->dy = 1.f;
            } else {
                vel->dy = 0.f;
            }
            // LEFT
            if (cmd & (1U << 2)) {
                vel->dx = -1.f;
            }
            // RIGHT
            else if (cmd & (1U << 3)) {
                vel->dx = 1.f;
            } else {
                vel->dx = 0.f;
            }
            state_stat = HANDLED_STATUS;
        } break;
        case EXIT_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
