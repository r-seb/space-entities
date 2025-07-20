#pragma once

#include "game/ecs.h"
#include "state_machine.h"
#include "u8g2.h"

#define DELTA_TIME_S 0.016f
#define ANIMATE_TIME_S 0.1f
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define MOVE_UP_CMD (1U << 0)
#define MOVE_DOWN_CMD (1U << 1)
#define MOVE_LEFT_CMD (1U << 2)
#define MOVE_RIGHT_CMD (1U << 3)

#define TOP_BOUND_REACHED (1U << 0)
#define BOTTOM_BOUND_REACHED (1U << 1)
#define LEFT_BOUND_REACHED (1U << 2)
#define RIGHT_BOUND_REACHED (1U << 3)

typedef struct {
    float x;
    float y;
    float radians;
} position_comp_t;

typedef struct {
    float dx;
    float dy;
} velocity_comp_t;

typedef struct {
    const uint8_t* sprites;
    float animate_time_s;
    float frame_time;
    uint8_t frame_size;
    uint8_t frame_idx;
    uint8_t frame_count;
    uint8_t width;
    uint8_t height;
} sprite_comp_t;

typedef struct {
    uint8_t health;
} health_comp_t;

typedef struct {
    Hsm super;
    ecs_world_t* ecs;
    ecs_entity_t entity;
} state_comp_t;

void game_init();
void game_system_tick(const Event* const e);
void game_system_move();
void game_system_keep_in_boundary();
void game_system_draw(u8g2_t* oled);
void game_system_animate();
void game_system_input(Event const* const e);
void game_system_collision_check();
void game_system_collision_resolution();
void game_system_remove_dead_entities();

// ---------------------------------------------------------------------------------------------//
// Entity State Machines
void sm_player_ctor_call(state_comp_t* sm_instance);
void sm_cauldron_ctor_call(state_comp_t* sm_instance);
void sm_mine_ctor_call(state_comp_t* sm_instance);
