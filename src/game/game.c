#include "game/game.h"
#include "game/assets.h"
#include "game/ecs.h"

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

    ecs_entity_t player_ent = ecs_create_entity(&ecs, (PLAYER_TAG | ANIMATE_TAG));
    ecs_entity_t cauldron_ent = ecs_create_entity(&ecs, (ENEMY_TAG | ANIMATE_TAG));
    ecs_entity_t crawler1_ent = ecs_create_entity(&ecs, (ENEMY_TAG | ANIMATE_TAG));
    ecs_entity_t crawler2_ent = ecs_create_entity(&ecs, (ENEMY_TAG | ANIMATE_TAG));

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
                            .frame_size = PLAYER_SHIP_BMP_FRAME_SIZE,
                            .frame_count = PLAYER_SHIP_BMP_FRAME_COUNT,
                            .width = PLAYER_SHIP_BMP_FRAME_WIDTH,
                            .height = PLAYER_SHIP_BMP_FRAME_HEIGHT};
    ecs_add_component(&ecs, player_ent, SPRITE_COMP_ID, &sprite);
    sprite = (sprite_comp_t) {.sprites = cauldron_bmp,
                              .frame_size = CAULDRON_BMP_FRAME_SIZE,
                              .frame_count = CAULDRON_BMP_FRAME_COUNT,
                              .width = CAULDRON_BMP_FRAME_WIDTH,
                              .height = CAULDRON_BMP_FRAME_HEIGHT};
    ecs_add_component(&ecs, cauldron_ent, SPRITE_COMP_ID, &sprite);
    sprite = (sprite_comp_t) {.sprites = crawler_bmp,
                              .frame_size = CRAWLER_BMP_FRAME_SIZE,
                              .frame_count = CRAWLER_BMP_FRAME_COUNT,
                              .width = CRAWLER_BMP_FRAME_WIDTH,
                              .height = CRAWLER_BMP_FRAME_HEIGHT};
    ecs_add_component(&ecs, crawler1_ent, SPRITE_COMP_ID, &sprite);
    ecs_add_component(&ecs, crawler2_ent, SPRITE_COMP_ID, &sprite);
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

void game_system_draw(u8g2_t* oled)
{
    uint8_t component_count = ecs.components[SPRITE_COMP_ID].set.count;
    u8g2_ClearBuffer(oled);
    for (int idx = 0; idx < component_count; ++idx) {
        sprite_comp_t* sp = ECS_GET_COMP_FROM_IDX(&ecs, SPRITE_COMP_ID, idx, sprite_comp_t);
        ecs_entity_t ent = ecs.components[POSITION_COMP_ID].set.dense[idx];
        position_comp_t* pos = ECS_GET_COMP_FROM_ENT(&ecs, POSITION_COMP_ID, ent, position_comp_t);

        u8g2_DrawXBM(oled, pos->x, pos->y, sp->width, sp->height,
                     &sp->sprites[sp->frame_idx * sp->frame_size]);
    }
    u8g2_SendBuffer(oled);
}

void game_system_animate()
{
    uint8_t component_count = ecs.components[SPRITE_COMP_ID].set.count;
    for (int idx = 0; idx < component_count; ++idx) {
        sprite_comp_t* sp = ECS_GET_COMP_FROM_IDX(&ecs, SPRITE_COMP_ID, idx, sprite_comp_t);
        ecs_entity_t ent = ecs.components[SPRITE_COMP_ID].set.dense[idx];

        if (ecs_get_entity_tag(&ecs, ent) & ANIMATE_TAG) {
            sp->frame_time += DELTA_TIME_S;

            if (sp->frame_time > ANIMATE_TIME_S) {
                sp->frame_idx = (sp->frame_idx + 1) % sp->frame_count;
                sp->frame_time = 0.f;
            }
        }
    }
}
