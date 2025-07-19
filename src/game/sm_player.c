#include "app.h"
#include "game/game.h"
#include "state_machine.h"

// Function prototypes
static void sm_player_ctor(state_comp_t* const me);
static State sm_player_initial(state_comp_t* const me, Event const* const e);
static State sm_player_flying(state_comp_t* const me, Event const* const e);
static State sm_player_exploding(state_comp_t* const me, Event const* const e);

// Private Variables
static float _invicible_time_s;

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
        case TIME_TICK_SIG: {
            if (_invicible_time_s > 0) {
                _invicible_time_s -= DELTA_TIME_S;
            } else {
                _invicible_time_s = 0;
                sprite_comp_t* sp =
                    ECS_GET_COMP_FROM_ENT(me->ecs, SPRITE_COMP_ID, me->entity, sprite_comp_t);
                sp->frame_count = 3;
                ecs_unset_entity_tag(me->ecs, me->entity, INVINCIBLE_TAG);
            }
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
        case COLLIDED_SIG: {
            // TODO: Decrease health
            if (_invicible_time_s == 0) {
                sprite_comp_t* sp =
                    ECS_GET_COMP_FROM_ENT(me->ecs, SPRITE_COMP_ID, me->entity, sprite_comp_t);
                sp->frame_count = 4;
                _invicible_time_s = 1.0f;
                ecs_set_entity_tag(me->ecs, me->entity, INVINCIBLE_TAG);
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
