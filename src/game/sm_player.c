#include "app.h"
#include "assets.h"
#include "game/game.h"
#include "state_machine.h"

#define SHOOT_INTERVAL_S 0.2f
#define INVINCIBLE_INTERVAL_S 3.f

// Function prototypes
static void sm_player_ctor(state_comp_t* const me);
static State sm_player_initial(state_comp_t* const me, Event const* const e);
static State sm_player_flying(state_comp_t* const me, Event const* const e);
static State sm_player_exploding(state_comp_t* const me, Event const* const e);

// Private Variables
static float _invicible_time_s;
static float _shooting_interval_s;

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
            if (_invicible_time_s > 0.f) {
                _invicible_time_s -= DELTA_TIME_S;
            } else {
                _invicible_time_s = 0;
                sprite_comp_t* sp =
                    ECS_GET_COMP_FROM_ENT(me->ecs, SPRITE_COMP_ID, me->entity, sprite_comp_t);
                sp->frame_count = 3;
                ecs_unset_entity_tag(me->ecs, me->entity, INVINCIBLE_TAG);
            }

            _shooting_interval_s -= DELTA_TIME_S;
            if (_shooting_interval_s <= 0.f) {
                ecs_entity_t bullet_ent = ecs_create_entity(me->ecs, BULLET_TAG);
                if (bullet_ent != UINT16_MAX) {
                    // Sprite component
                    sprite_comp_t sp = {.sprites = player_bullet_bmp,
                                        .animate_time_s = ANIMATE_TIME_S,
                                        .frame_size = PLAYER_BULLET_BMP_FRAME_SIZE,
                                        .frame_count = PLAYER_BULLET_BMP_FRAME_COUNT,
                                        .width = PLAYER_BULLET_BMP_FRAME_WIDTH,
                                        .height = PLAYER_BULLET_BMP_FRAME_HEIGHT};
                    ecs_add_component(me->ecs, bullet_ent, SPRITE_COMP_ID, &sp);

                    // State Machine component
                    state_comp_t sm = (state_comp_t) {.ecs = me->ecs, .entity = bullet_ent};
                    sm_player_bullet_ctor_call(&sm);
                    ecs_add_component(me->ecs, sm.entity, STATE_COMP_ID, &sm);

                    position_comp_t* player_pos = ECS_GET_COMP_FROM_ENT(
                        me->ecs, POSITION_COMP_ID, me->entity, position_comp_t);
                    // Position component
                    position_comp_t pos = {.x = player_pos->x + sp.width + 4,
                                           .y = player_pos->y + sp.height + 1};
                    ecs_add_component(me->ecs, bullet_ent, POSITION_COMP_ID, &pos);

                    // Velocity component
                    velocity_comp_t vel = {.dx = 2.0f, .dy = 0.f};
                    ecs_add_component(me->ecs, bullet_ent, VELOCITY_COMP_ID, &vel);

                    _shooting_interval_s = SHOOT_INTERVAL_S;
                }
            }
            state_stat = HANDLED_STATUS;
        } break;
        case SHIP_MOVE_SIG: {
            uint8_t cmd = EVENT_CAST(e, PositionEvent)->move_cmd;
            velocity_comp_t* vel =
                ECS_GET_COMP_FROM_ENT(me->ecs, VELOCITY_COMP_ID, me->entity, velocity_comp_t);

            if (cmd & MOVE_UP_CMD) {
                vel->dy = -1.f;
            } else if (cmd & MOVE_DOWN_CMD) {
                vel->dy = 1.f;
            } else {
                vel->dy = 0.f;
            }

            if (cmd & MOVE_LEFT_CMD) {
                vel->dx = -1.f;
            } else if (cmd & MOVE_RIGHT_CMD) {
                vel->dx = 1.f;
            } else {
                vel->dx = 0.f;
            }
            state_stat = HANDLED_STATUS;
        } break;
        case COLLIDED_SIG: {
            if (_invicible_time_s <= 0.f) {
                // Decrease health
                health_comp_t* hp =
                    ECS_GET_COMP_FROM_ENT(me->ecs, HEALTH_COMP_ID, me->entity, health_comp_t);
                hp->health = (hp->health == 0) ? 3 : (hp->health - 1);

                // Set to invisible for a period of time
                sprite_comp_t* sp =
                    ECS_GET_COMP_FROM_ENT(me->ecs, SPRITE_COMP_ID, me->entity, sprite_comp_t);
                sp->frame_count = 4;
                _invicible_time_s = INVINCIBLE_INTERVAL_S;
                ecs_set_entity_tag(me->ecs, me->entity, INVINCIBLE_TAG);
            }
            state_stat = HANDLED_STATUS;
        } break;
        case BOUND_REACHED_SIG: {
            position_comp_t* pos =
                ECS_GET_COMP_FROM_ENT(me->ecs, POSITION_COMP_ID, me->entity, position_comp_t);
            sprite_comp_t* sp =
                ECS_GET_COMP_FROM_ENT(me->ecs, SPRITE_COMP_ID, me->entity, sprite_comp_t);
            uint8_t bounds = EVENT_CAST(e, BoundEvent)->bound_bitmask;

            // X-Axis
            if (bounds & LEFT_BOUND_REACHED) {
                pos->x = 0.f;
            } else if (bounds & RIGHT_BOUND_REACHED) {
                pos->x = OLED_WIDTH - sp->width;
            }

            // Y-Axis
            if (bounds & TOP_BOUND_REACHED) {
                pos->y = 0.f;
            } else if (bounds & BOTTOM_BOUND_REACHED) {
                pos->y = OLED_HEIGHT - sp->height;
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
