#include "app.h"
#include "game/game.h"
#include "state_machine.h"

// Function prototypes
static void sm_crawler_ctor(state_comp_t* const me);
static State sm_crawler_initial(state_comp_t* const me, Event const* const e);
static State sm_crawler_bouncing(state_comp_t* const me, Event const* const e);

// To be called by main
void sm_crawler_ctor_call(state_comp_t* sm_instance)
{
    sm_crawler_ctor(sm_instance);
}

static void sm_crawler_ctor(state_comp_t* const me)
{
    Hsm_ctor(&me->super, (StateHandler)&sm_crawler_initial);
}

static State sm_crawler_initial(state_comp_t* me, Event const* const e)
{
    return TRAN(&sm_crawler_bouncing);
}

static State sm_crawler_bouncing(state_comp_t* const me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        case TIME_TICK_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        case COLLIDED_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        case BOUND_REACHED_SIG: {
            position_comp_t* pos =
                ECS_GET_COMP_FROM_ENT(me->ecs, POSITION_COMP_ID, me->entity, position_comp_t);
            sprite_comp_t* sp =
                ECS_GET_COMP_FROM_ENT(me->ecs, SPRITE_COMP_ID, me->entity, sprite_comp_t);
            velocity_comp_t* vel =
                ECS_GET_COMP_FROM_ENT(me->ecs, VELOCITY_COMP_ID, me->entity, velocity_comp_t);
            uint8_t bounds = EVENT_CAST(e, BoundEvent)->bound_bitmask;

            // X-Axis
            if (bounds & LEFT_BOUND_REACHED) {
                pos->x = 0.f;
                vel->dx *= -1.f;
            } else if (bounds & RIGHT_BOUND_REACHED) {
                pos->x = OLED_WIDTH - sp->width;
                vel->dx *= -1.f;
            }

            // Y-Axis
            if (bounds & TOP_BOUND_REACHED) {
                pos->y = 0.f;
                vel->dy *= -1.f;
            } else if (bounds & BOTTOM_BOUND_REACHED) {
                pos->y = OLED_HEIGHT - sp->height;
                vel->dy *= -1.f;
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
