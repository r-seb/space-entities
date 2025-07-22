#include "app.h"
#include "game/assets.h"
#include "game/ecs.h"
#include "game/game.h"
#include "state_machine.h"
#include "uart.h"

// Function prototypes
static void sm_glyph_ctor(state_comp_t* const me);
static State sm_glyph_initial(state_comp_t* const me, Event const* const e);
static State sm_glyph_drifting(state_comp_t* const me, Event const* const e);

// To be called by main
void sm_glyph_ctor_call(state_comp_t* sm_instance)
{
    sm_glyph_ctor(sm_instance);
}

static void sm_glyph_ctor(state_comp_t* const me)
{
    Hsm_ctor(&me->super, (StateHandler)&sm_glyph_initial);
}

static State sm_glyph_initial(state_comp_t* me, Event const* const e)
{
    return TRAN(&sm_glyph_drifting);
}

static State sm_glyph_drifting(state_comp_t* const me, Event const* const e)
{
    State state_stat;
    switch (e->sig) {
        case TIME_TICK_SIG: {
            position_comp_t* pos =
                ECS_GET_COMP_FROM_ENT(me->ecs, POSITION_COMP_ID, me->entity, position_comp_t);
            sprite_comp_t* sp =
                ECS_GET_COMP_FROM_ENT(me->ecs, SPRITE_COMP_ID, me->entity, sprite_comp_t);
            velocity_comp_t* vel =
                ECS_GET_COMP_FROM_ENT(me->ecs, VELOCITY_COMP_ID, me->entity, velocity_comp_t);

            if ((pos->x < -sp->width) || (pos->x > (OLED_WIDTH + sp->width)) ||
                (pos->y < -sp->height) || (pos->y > (OLED_HEIGHT + sp->height))) {
                ecs_set_entity_tag(me->ecs, me->entity, DEAD_TAG);
                // uart_send("Entity %u marked DEAD\n\r", me->entity >> ENTITY_BITSHIFT);
            }

            // Mark as dead once the explosion animation ends
            if (vel->dx == 0.f && vel->dy == 0.f && sp->frame_idx == sp->frame_count - 1) {
                ecs_set_entity_tag(me->ecs, me->entity, DEAD_TAG);
            }

            state_stat = HANDLED_STATUS;
        } break;
        case COLLIDED_SIG: {
            sprite_comp_t* sp =
                ECS_GET_COMP_FROM_ENT(me->ecs, SPRITE_COMP_ID, me->entity, sprite_comp_t);
            sp->sprites = glyph_exploded_bmp;
            sp->frame_count = GLYPH_EXPLODED_BMP_FRAME_COUNT;
            sp->frame_size = GLYPH_BMP_FRAME_SIZE;
            sp->height = GLYPH_BMP_FRAME_HEIGHT;
            sp->width = GLYPH_BMP_FRAME_WIDTH;
            sp->animate_time_s = ANIMATE_TIME_S;

            velocity_comp_t* vel =
                ECS_GET_COMP_FROM_ENT(me->ecs, VELOCITY_COMP_ID, me->entity, velocity_comp_t);
            vel->dx = 0.f;
            vel->dy = 0.f;

            state_stat = HANDLED_STATUS;
        } break;
        default:
            state_stat = IGNORED_STATUS;
            break;
    }
    return state_stat;
}
