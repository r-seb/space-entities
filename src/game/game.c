#include "game/game.h"
#include "app.h"
#include "game/assets.h"
#include "game/ecs.h"
#include "state_machine.h"

static ecs_component_t components[COMP_ID_MAX];

static ecs_entity_t position_comp_sparse[MAX_ENTITIES];
static ecs_entity_t position_comp_dense[MAX_ENTITIES];
static position_comp_t position_comp_data[MAX_ENTITIES];

static ecs_entity_t velocity_comp_sparse[MAX_ENTITIES];
static ecs_entity_t velocity_comp_dense[MAX_ENTITIES];
static velocity_comp_t velocity_comp_data[MAX_ENTITIES];

static ecs_entity_t health_comp_sparse[MAX_ENTITIES];
static ecs_entity_t health_comp_dense[MAX_ENTITIES];
static health_comp_t health_comp_data[MAX_ENTITIES];

static ecs_entity_t sprite_comp_sparse[MAX_ENTITIES];
static ecs_entity_t sprite_comp_dense[MAX_ENTITIES];
static sprite_comp_t sprite_comp_data[MAX_ENTITIES];

static ecs_entity_t state_comp_sparse[MAX_ENTITIES];
static ecs_entity_t state_comp_dense[MAX_ENTITIES];
static state_comp_t state_comp_data[MAX_ENTITIES];

static ecs_entity_t entities_sparse[MAX_ENTITIES];
static ecs_entity_t entities_dense[MAX_ENTITIES];

static ecs_world_t ecs = {0};

void game_init()
{
    ecs.components = components;
    // Position
    ecs.components[POSITION_COMP_ID].set.sparse = position_comp_sparse;
    ecs.components[POSITION_COMP_ID].set.dense = position_comp_dense;
    ecs.components[POSITION_COMP_ID].data = position_comp_data;
    ecs.components[POSITION_COMP_ID].data_size = sizeof(position_comp_t);
    // velocity
    ecs.components[VELOCITY_COMP_ID].set.sparse = velocity_comp_sparse;
    ecs.components[VELOCITY_COMP_ID].set.dense = velocity_comp_dense;
    ecs.components[VELOCITY_COMP_ID].data = velocity_comp_data;
    ecs.components[VELOCITY_COMP_ID].data_size = sizeof(velocity_comp_t);
    // Health
    ecs.components[HEALTH_COMP_ID].set.sparse = health_comp_sparse;
    ecs.components[HEALTH_COMP_ID].set.dense = health_comp_dense;
    ecs.components[HEALTH_COMP_ID].data = health_comp_data;
    ecs.components[HEALTH_COMP_ID].data_size = sizeof(health_comp_t);
    // Sprite
    ecs.components[SPRITE_COMP_ID].set.sparse = sprite_comp_sparse;
    ecs.components[SPRITE_COMP_ID].set.dense = sprite_comp_dense;
    ecs.components[SPRITE_COMP_ID].data = sprite_comp_data;
    ecs.components[SPRITE_COMP_ID].data_size = sizeof(sprite_comp_t);
    // State
    ecs.components[STATE_COMP_ID].set.sparse = state_comp_sparse;
    ecs.components[STATE_COMP_ID].set.dense = state_comp_dense;
    ecs.components[STATE_COMP_ID].data = state_comp_data;
    ecs.components[STATE_COMP_ID].data_size = sizeof(state_comp_t);

    // Entities
    ecs.entities.sparse = entities_sparse;
    ecs.entities.dense = entities_dense;

    ecs_entity_t player_ent = ecs_create_entity(&ecs, (PLAYER_TAG | KEEP_BOUNDED_TAG));
    ecs_entity_t cauldron_ent = ecs_create_entity(&ecs, (ENEMY_TAG));
    ecs_entity_t crawler1_ent = ecs_create_entity(&ecs, (ENEMY_TAG));
    ecs_entity_t crawler2_ent = ecs_create_entity(&ecs, (ENEMY_TAG));

    position_comp_t pos = {.x = 30.f, .y = 28.f};
    ecs_add_component(&ecs, player_ent, POSITION_COMP_ID, &pos);
    pos = (position_comp_t) {.x = 100.f, .y = 22.f};
    ecs_add_component(&ecs, cauldron_ent, POSITION_COMP_ID, &pos);
    pos = (position_comp_t) {.x = 117.f, .y = 5.f};
    ecs_add_component(&ecs, crawler1_ent, POSITION_COMP_ID, &pos);
    pos = (position_comp_t) {.x = 117.f, .y = 49.f};
    ecs_add_component(&ecs, crawler2_ent, POSITION_COMP_ID, &pos);

    velocity_comp_t vel = {.dx = 0.f, .dy = 0.f};
    ecs_add_component(&ecs, player_ent, VELOCITY_COMP_ID, &vel);
    vel = (velocity_comp_t) {.dx = 0.f, .dy = 0.f};
    ecs_add_component(&ecs, cauldron_ent, VELOCITY_COMP_ID, &vel);
    ecs_add_component(&ecs, crawler1_ent, VELOCITY_COMP_ID, &vel);
    ecs_add_component(&ecs, crawler2_ent, VELOCITY_COMP_ID, &vel);

    sprite_comp_t sprite = {.sprites = player_ship_bmp,
                            .animate_time_s = ANIMATE_TIME_S,
                            .frame_size = PLAYER_SHIP_BMP_FRAME_SIZE,
                            .frame_count = PLAYER_SHIP_BMP_FRAME_COUNT,
                            .width = PLAYER_SHIP_BMP_FRAME_WIDTH,
                            .height = PLAYER_SHIP_BMP_FRAME_HEIGHT};
    ecs_add_component(&ecs, player_ent, SPRITE_COMP_ID, &sprite);
    sprite = (sprite_comp_t) {.sprites = cauldron_bmp,
                              .animate_time_s = ANIMATE_TIME_S,
                              .frame_size = CAULDRON_BMP_FRAME_SIZE,
                              .frame_count = CAULDRON_BMP_FRAME_COUNT,
                              .width = CAULDRON_BMP_FRAME_WIDTH,
                              .height = CAULDRON_BMP_FRAME_HEIGHT};
    ecs_add_component(&ecs, cauldron_ent, SPRITE_COMP_ID, &sprite);
    sprite = (sprite_comp_t) {.sprites = crawler_bmp,
                              .animate_time_s = ANIMATE_TIME_S,
                              .frame_size = CRAWLER_BMP_FRAME_SIZE,
                              .frame_count = CRAWLER_BMP_FRAME_COUNT,
                              .width = CRAWLER_BMP_FRAME_WIDTH,
                              .height = CRAWLER_BMP_FRAME_HEIGHT};
    ecs_add_component(&ecs, crawler1_ent, SPRITE_COMP_ID, &sprite);
    ecs_add_component(&ecs, crawler2_ent, SPRITE_COMP_ID, &sprite);

    // Initialize all state machines
    state_comp_t sm = (state_comp_t) {.ecs = &ecs, .entity = player_ent};
    sm_player_ctor_call(&sm);
    ecs_add_component(&ecs, player_ent, STATE_COMP_ID, &sm);
}

void game_system_tick(const Event* const e)
{
    uint8_t sm_count = ecs.components[STATE_COMP_ID].set.count;
    for (int idx = 0; idx < sm_count; ++idx) {
        state_comp_t* sm = ECS_GET_COMP_FROM_IDX(&ecs, STATE_COMP_ID, idx, state_comp_t);
        // Dispatch system tick to every entities with state machine
        Hsm_dispatch(&sm->super, e);
    }
}

void game_system_move()
{
    uint8_t component_count = ecs.components[POSITION_COMP_ID].set.count;
    for (int idx = 0; idx < component_count; ++idx) {
        position_comp_t* pos = ECS_GET_COMP_FROM_IDX(&ecs, POSITION_COMP_ID, idx, position_comp_t);
        ecs_entity_t ent = ecs.components[POSITION_COMP_ID].set.dense[idx];
        velocity_comp_t* vel = ECS_GET_COMP_FROM_ENT(&ecs, VELOCITY_COMP_ID, ent, velocity_comp_t);

        pos->x += vel->dx;
        pos->y += vel->dy;
    }
}

void game_system_keep_in_boundary()
{
    uint8_t component_count = ecs.components[POSITION_COMP_ID].set.count;
    for (int idx = 0; idx < component_count; ++idx) {
        position_comp_t* pos = ECS_GET_COMP_FROM_IDX(&ecs, POSITION_COMP_ID, idx, position_comp_t);
        ecs_entity_t ent = ecs.components[POSITION_COMP_ID].set.dense[idx];
        sprite_comp_t* sp = ECS_GET_COMP_FROM_ENT(&ecs, SPRITE_COMP_ID, ent, sprite_comp_t);

        if (ecs_get_entity_tag(&ecs, ent) & KEEP_BOUNDED_TAG) {
            if (pos->x < 0.f) {
                pos->x = 0;
            } else if (pos->x > (OLED_WIDTH - sp->width)) {
                pos->x = OLED_WIDTH - sp->width;
            }

            if (pos->y < 0.f) {
                pos->y = 0;
            } else if (pos->y > (OLED_HEIGHT - sp->height)) {
                pos->y = OLED_HEIGHT - sp->height;
            }

            // TODO: Hsm_dispatch BOUND_REACHED_SIG
        }
    }
}

void game_system_draw(u8g2_t* oled)
{
    uint8_t component_count = ecs.components[SPRITE_COMP_ID].set.count;
    u8g2_ClearBuffer(oled);
    for (int idx = 0; idx < component_count; ++idx) {
        sprite_comp_t* sp = ECS_GET_COMP_FROM_IDX(&ecs, SPRITE_COMP_ID, idx, sprite_comp_t);
        ecs_entity_t ent = ecs.components[POSITION_COMP_ID].set.dense[idx];
        position_comp_t* pos = ECS_GET_COMP_FROM_ENT(&ecs, POSITION_COMP_ID, ent, position_comp_t);

        // Draw starts --> +---------+
        //  from here      |         |
        //                 |   BMP   |
        //                 |         |
        //                 +---------+
        // HACK: Cast to signed int to support drawing bitmaps offscreen at coordinates < 0
        u8g2_DrawXBM(oled, (int16_t)pos->x, (int16_t)pos->y, sp->width, sp->height,
                     &sp->sprites[sp->frame_idx * sp->frame_size]);
    }
    u8g2_SendBuffer(oled);
}

void game_system_animate()
{
    uint8_t component_count = ecs.components[SPRITE_COMP_ID].set.count;
    for (int idx = 0; idx < component_count; ++idx) {
        sprite_comp_t* sp = ECS_GET_COMP_FROM_IDX(&ecs, SPRITE_COMP_ID, idx, sprite_comp_t);

        if (sp->frame_count > 1) {
            sp->frame_time += DELTA_TIME_S;

            if (sp->frame_time > sp->animate_time_s) {
                sp->frame_idx = (sp->frame_idx + 1) % sp->frame_count;
                sp->frame_time = 0.f;
            }
        }
    }
}

void game_system_input(Event const* const e)
{
    uint8_t component_count = ecs.components[VELOCITY_COMP_ID].set.count;
    for (int idx = 0; idx < component_count; ++idx) {
        ecs_entity_t ent = ecs.components[VELOCITY_COMP_ID].set.dense[idx];
        state_comp_t* sm = ECS_GET_COMP_FROM_ENT(&ecs, STATE_COMP_ID, ent, state_comp_t);

        if (ecs_get_entity_tag(&ecs, ent) & PLAYER_TAG) {
            Hsm_dispatch(&sm->super, e);
            break;
        }
    }
}

void game_system_collision_check()
{
    uint8_t component_count = ecs.components[POSITION_COMP_ID].set.count;
    for (int i = 0; i < component_count; ++i) {
        position_comp_t* pos_a = ECS_GET_COMP_FROM_IDX(&ecs, POSITION_COMP_ID, i, position_comp_t);
        ecs_entity_t ent_a = ecs.components[POSITION_COMP_ID].set.dense[i];
        sprite_comp_t* sp = ECS_GET_COMP_FROM_ENT(&ecs, SPRITE_COMP_ID, ent_a, sprite_comp_t);
        velocity_comp_t* vel_a =
            ECS_GET_COMP_FROM_ENT(&ecs, VELOCITY_COMP_ID, ent_a, velocity_comp_t);

        // NOTE: Tags that doesn't need collision check since it's not destroyable
        ecs_entity_t tag_a = ecs_get_entity_tag(&ecs, ent_a);
        if (tag_a & (OBSTACLE_TAG | INVINCIBLE_TAG)) {
            continue;
        }

        float reca_top_left_x = pos_a->x;
        float reca_top_left_y = pos_a->y;
        float reca_bot_right_x = pos_a->x + sp->width;
        float reca_bot_right_y = pos_a->y + sp->height;

        for (int j = 0; j < component_count; ++j) {
            position_comp_t* pos_b =
                ECS_GET_COMP_FROM_IDX(&ecs, POSITION_COMP_ID, j, position_comp_t);
            ecs_entity_t ent_b = ecs.components[POSITION_COMP_ID].set.dense[j];
            sp = ECS_GET_COMP_FROM_ENT(&ecs, SPRITE_COMP_ID, ent_b, sprite_comp_t);

            // Checks to skip, X = YES, O = NO
            // TAGS a\b |   PLAYER  ENEMY   BULLET  OBSTACLE
            // ---------+-----------------------------------
            // PLAYER   |     X       O       X        O
            // ENEMY    |     X       X       O        X
            // BULLET   |     X       O       X        O
            ecs_entity_t tag_b = ecs_get_entity_tag(&ecs, ent_b);
            if (ent_a == ent_b || (tag_a & PLAYER_TAG && tag_b & BULLET_TAG) ||
                (tag_a & ENEMY_TAG && (tag_b & OBSTACLE_TAG || tag_b & ENEMY_TAG)) ||
                (tag_a & ENEMY_TAG && (tag_b & PLAYER_TAG && tag_b & INVINCIBLE_TAG)) ||
                (tag_a & BULLET_TAG && (tag_b & PLAYER_TAG || tag_b & BULLET_TAG))) {
                continue;
            }

            float recb_top_left_x = pos_b->x;
            float recb_top_left_y = pos_b->y;
            float recb_bot_right_x = pos_b->x + sp->width;
            float recb_bot_right_y = pos_b->y + sp->height;

            // Check collision, AABB method
            // +---------+
            // |         |
            // |    A +--|------+
            // |      |//|      |
            // +------|--+ B    |
            //        |         |
            //        +---------+
            bool collided = (reca_top_left_x <= recb_bot_right_x) &&
                (reca_bot_right_x >= recb_top_left_x) && (reca_top_left_y <= recb_bot_right_y) &&
                (reca_bot_right_y >= recb_top_left_y);
            if (collided) {
                ecs_set_entity_tag(&ecs, ent_a, COLLIDED_TAG);

                // Compute the overlap on each axis
                float overlap_x =
                    (reca_bot_right_x - recb_top_left_x < recb_bot_right_x - reca_top_left_x)
                    ? (reca_bot_right_x - recb_top_left_x)
                    : (recb_bot_right_x - reca_top_left_x);
                float overlap_y =
                    (reca_bot_right_y - recb_top_left_y < recb_bot_right_y - reca_top_left_y)
                    ? (reca_bot_right_y - recb_top_left_y)
                    : (recb_bot_right_y - reca_top_left_y);

                // Resolve on the axis with the least overlap
                // This somewhat prevents collided objects from phasing through each other but it
                // probably doesn't prevent tunneling, maybe try the swept AABB method?
                if (overlap_y < overlap_x) {
                    // Resolve along y axis
                    if (vel_a->dy > 0.f) {
                        pos_a->y -= overlap_y;
                    } else if (vel_a->dy < 0.f) {
                        pos_a->y += overlap_y;
                    }
                } else {
                    // Resolve along x axis
                    if (vel_a->dx > 0.f) {
                        pos_a->x -= overlap_x;
                    } else if (vel_a->dx < 0.f) {
                        pos_a->x += overlap_x;
                    }
                }
            }
        }
    }
}

void game_system_collision_resolution()
{
    uint8_t sm_count = ecs.components[STATE_COMP_ID].set.count;
    for (int idx = 0; idx < sm_count; ++idx) {
        state_comp_t* sm = ECS_GET_COMP_FROM_IDX(&ecs, STATE_COMP_ID, idx, state_comp_t);
        ecs_entity_t ent = ecs.components[STATE_COMP_ID].set.dense[idx];

        if (ecs_get_entity_tag(&ecs, ent) & COLLIDED_TAG) {
            ecs_unset_entity_tag(&ecs, ent, COLLIDED_TAG);
            static const Event collided_evt = {COLLIDED_SIG};
            Hsm_dispatch(&sm->super, &collided_evt);
        }
    }
}
