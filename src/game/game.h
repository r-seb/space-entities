#pragma once

#include "state_machine.h"
#include "u8g2.h"

#define DELTA_TIME_S 0.016f
#define ANIMATE_TIME_S 0.1f
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

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
} state_comp_t;

void game_init();
void game_system_move();
void game_system_draw(u8g2_t* oled);
void game_system_animate();
