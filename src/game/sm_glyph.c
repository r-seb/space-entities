#include "app.h"
#include "game/ecs.h"
#include "game/game.h"
#include "state_machine.h"
#include "uart.h"

// Function prototypes
static void sm_mine_ctor(state_comp_t* const me);
static State sm_mine_initial(state_comp_t* const me, Event const* const e);
static State sm_mine_drifting(state_comp_t* const me, Event const* const e);

// To be called by main
void sm_mine_ctor_call(state_comp_t* sm_instance)
{
    sm_mine_ctor(sm_instance);
}

static void sm_mine_ctor(state_comp_t* const me)
{
    Hsm_ctor(&me->super, (StateHandler)&sm_mine_initial);
}

static State sm_mine_initial(state_comp_t* me, Event const* const e)
{
    return TRAN(&sm_mine_drifting);
}

static State sm_mine_drifting(state_comp_t* const me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case TIME_TICK_SIG: {
            position_comp_t* pos =
                ECS_GET_COMP_FROM_ENT(me->ecs, POSITION_COMP_ID, me->entity, position_comp_t);
            sprite_comp_t* sp =
                ECS_GET_COMP_FROM_ENT(me->ecs, SPRITE_COMP_ID, me->entity, sprite_comp_t);

            if ((pos->x < 0.f) || (pos->x > (OLED_WIDTH - sp->width)) || (pos->y < 0.f) ||
                (pos->y > (OLED_HEIGHT - sp->height))) {
                ecs_set_entity_tag(me->ecs, me->entity, DEAD_TAG);
                // uart_send("Entity %u marked DEAD\n\r", me->entity >> ENTITY_BITSHIFT);
            }

            state_stat = HANDLED_STATUS;
        } break;
        case COLLIDED_SIG: {
            ecs_set_entity_tag(me->ecs, me->entity, DEAD_TAG);
            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
