#include "app.h"
#include "assets.h"
#include "game/ecs.h"
#include "game/game.h"
#include "state_machine.h"
#include <stdint.h>

#define INTERVAL_S 0.5f

// Function prototypes
static void sm_cauldron_ctor(state_comp_t* const me);
static State sm_cauldron_initial(state_comp_t* const me, Event const* const e);
static State sm_cauldron_shooting(state_comp_t* const me, Event const* const e);

// Private Variables
static float _shooting_interval_s;

// To be called by main
void sm_cauldron_ctor_call(state_comp_t* sm_instance)
{
    sm_cauldron_ctor(sm_instance);
}

static void sm_cauldron_ctor(state_comp_t* const me)
{
    Hsm_ctor(&me->super, (StateHandler)&sm_cauldron_initial);
}

static State sm_cauldron_initial(state_comp_t* me, Event const* const e)
{
    return TRAN(&sm_cauldron_shooting);
}

static State sm_cauldron_shooting(state_comp_t* const me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case ENTRY_SIG: {
            state_stat = HANDLED_STATUS;
        } break;
        case TIME_TICK_SIG: {
            _shooting_interval_s -= DELTA_TIME_S;
            if (_shooting_interval_s <= 0.f) {
                ecs_entity_t mine_ent = ecs_create_entity(me->ecs, ENEMY_TAG);
                if (mine_ent != UINT16_MAX) {
                    // Sprite component
                    sprite_comp_t sp = {.sprites = mine_bmp,
                                        .animate_time_s = ANIMATE_TIME_S,
                                        .frame_size = MINE_BMP_FRAME_SIZE,
                                        .frame_count = MINE_BMP_FRAME_COUNT,
                                        .width = MINE_BMP_FRAME_WIDTH,
                                        .height = MINE_BMP_FRAME_HEIGHT};
                    ecs_add_component(me->ecs, mine_ent, SPRITE_COMP_ID, &sp);

                    // State Machine component
                    state_comp_t sm = (state_comp_t) {.ecs = me->ecs, .entity = mine_ent};
                    sm_mine_ctor_call(&sm);
                    ecs_add_component(me->ecs, sm.entity, STATE_COMP_ID, &sm);

                    position_comp_t* cauldron_pos = ECS_GET_COMP_FROM_ENT(
                        me->ecs, POSITION_COMP_ID, me->entity, position_comp_t);
                    // Position component
                    position_comp_t pos = {.x = cauldron_pos->x + sp.width / 2.f,
                                           .y = cauldron_pos->y + sp.height / 2.f};
                    ecs_add_component(me->ecs, mine_ent, POSITION_COMP_ID, &pos);

                    // Velocity component
                    velocity_comp_t vel = {0};
                    // Find the player position
                    uint8_t pos_count = me->ecs->components[POSITION_COMP_ID].set.count;
                    for (int idx = 0; idx < pos_count; ++idx) {
                        ecs_entity_t ent = me->ecs->components[POSITION_COMP_ID].set.dense[idx];

                        if (ecs_get_entity_tag(me->ecs, ent) & PLAYER_TAG) {
                            position_comp_t* player_pos = ECS_GET_COMP_FROM_IDX(
                                me->ecs, POSITION_COMP_ID, idx, position_comp_t);
                            vel.dx = (player_pos->x - pos.x) * DELTA_TIME_S;
                            vel.dy = (player_pos->y - pos.y) * DELTA_TIME_S;
                        }
                    }
                    ecs_add_component(me->ecs, mine_ent, VELOCITY_COMP_ID, &vel);
                }

                _shooting_interval_s = INTERVAL_S;
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
