#pragma once

#include <stdint.h>

// 10x8                  10x8                  10x8
// . . . . X X X X . .   . . . . X X X X . .   . . . . X X X X . .
// . . X . . X . . X X   . . X . . X . . X X   . . X . . X . . X X
// . . . X X X X . . .   . X . X X X X . . .   . . . X X X X . . .
// . . X X . . . X . .   . X X X . . . X . .   X . X X . . . X . .
// . . X X . . . X . .   . X X X . . . X . .   X . X X . . . X . .
// . . . X X X X . . .   . X . X X X X . . .   . . . X X X X . . .
// . . X . . X . . X X   . . X . . X . . X X   . . X . . X . . X X
// . . . . X X X X . .   . . . . X X X X . .   . . . . X X X X . .
#define PLAYER_SHIP_BMP_FRAME_SIZE 16U
#define PLAYER_SHIP_BMP_FRAME_COUNT 3U
#define PLAYER_SHIP_BMP_FRAME_WIDTH 10U
#define PLAYER_SHIP_BMP_FRAME_HEIGHT 8U
extern const uint8_t player_ship_bmp[];

// 3x2     3x2
// . X X   X X .
// X X .   . X X
#define PLAYER_BULLET_BMP_FRAME_SIZE 2U
#define PLAYER_BULLET_BMP_FRAME_COUNT 2U
#define PLAYER_BULLET_BMP_FRAME_WIDTH 3U
#define PLAYER_BULLET_BMP_FRAME_HEIGHT 2U
extern const uint8_t player_bullet_bmp[];

// 12x13
// . . . . X X X . . . . .
// . . . X X X X X X X X .
// . . X X X X X . . X X X
// X X X X X X . . . . . .
// . X X X X X X . . . . .
// X . X . . . X X . . . .
// X X X . . . . . X . . .
// X . X . . . X X . . . .
// . X X X X X X . . . . .
// X X X X X X . . . . . .
// . . X X X X X X . X X X
// . . . X X X X X X X X .
// . . . . X X X . . . . .
#define PLAYER_SHIP2_BMP_FRAME_SIZE 26U
#define PLAYER_SHIP2_BMP_FRAME_COUNT 1U
#define PLAYER_SHIP2_BMP_FRAME_WIDTH 12U
#define PLAYER_SHIP2_BMP_FRAME_HEIGHT 13U
extern const uint8_t player_ship2_bmp[];

// 16x16
// . . . . . . . . . X X X X X X .
// . . . . . . . . . . . X X . . .
// . . . . . . . . . . . X X . . .
// . . . . . . . . X X X X X X X .
// . . . . . . . . . X X X X X . .
// . . . . X X X X X X X X X . . .
// . . X X X . . . X X X X . . X X
// X X X X . . . . . X X X X X X .
// X X X X . . . . . X X X X X X .
// . . X X X . . . X X X X . . X X
// . . . . X X X X X X X X X . . .
// . . . . . . . . . X X X X X . .
// . . . . . . . X X X X X X X X .
// . . . . . . . . . . . X X . . .
// . . . . . . . . . . . X X . . .
// . . . . . . . . X X X X X X X .
#define BOSS_SHIP_BMP_FRAME_SIZE 32U
#define BOSS_SHIP_BMP_FRAME_COUNT 1U
#define BOSS_SHIP_BMP_FRAME_WIDTH 16U
#define BOSS_SHIP_BMP_FRAME_HEIGHT 16U
extern const uint8_t boss_ship_bmp[];

// 7x7             7x7
// . . . X . . .   . . X . X . .
// . X . . . X .   . . . . . . .
// . . X X X . .   X . X X X . X
// X . X X X . X   . . X X X . .
// . . X X X . .   X . X X X . X
// . X . . . X .   . . . . . . .
// . . . X . . .   . . X . X . .
#define MINE_BMP_FRAME_SIZE 7U
#define MINE_BMP_FRAME_COUNT 2U
#define MINE_BMP_FRAME_WIDTH 7U
#define MINE_BMP_FRAME_HEIGHT 7U
extern const uint8_t mine_bmp[];

// 11x15                   11x15
// . . . . X . X . . . .   . . . . X . X . . . .
// . . . . . X . . . . .   . . . . . X . . . . .
// . X X . X X X . . . .   . . . . X X X . X X .
// . . X . X . X . . X X   X X . . X . X . X . .
// . . X . X X X . . X .   . X . . X X X . X . .
// . . X . . . . . . X .   . X . . . . . . X . .
// . . X X X X X X X X .   . X X X X X X X X . .
// . . X X X . X X X . .   . . X X X . X X X . .
// . . X X . . . X X . .   . . X X . . . X X . .
// . . X X X . X X X . .   . . X X X . X X X . .
// . X X X X X X X X . .   . . X X X X X X X X .
// . X . . . . . . X . .   . . X . . . . . . X .
// . X . . . . . . X . .   . . X . . . . . . X .
// X X . . . . . . X . .   . . X . . . . . . X X
// . . . . . . . . X X .   . X X . . . . . . . .
#define CRAWLER_BMP_FRAME_SIZE 30U
#define CRAWLER_BMP_FRAME_COUNT 2U
#define CRAWLER_BMP_FRAME_WIDTH 11U
#define CRAWLER_BMP_FRAME_HEIGHT 15U
extern const uint8_t crawler_bmp[];

// 15x20                             15x20                             15x20
// . . . . . . X . . . . . . . .     . . . . . . . . . . . . . . .     . . . . . . . . . . . . . . .
// . . . . . . X . . X . . . . .     . . . X . . . . . . . . . . .     . . . . . . . . . X . . . . .
// . . . . . X . X . . . . . . .     . X . . X X . . . . . . . . .     . . . . X . . . X . . . . . .
// . . . X . . X . X . . . . . .     . . . . X . . . . . . . . . .     . . . . . . . X . . . . . . .
// . . X . . . X X . . . . . . .     . . . X . . X X . . . . . . .     . . . . X . . . X . . . . . .
// . . . . X . . X X . . . . . .     . X . . X . . . . . . . . . .     . . . . . . . X . . . . . . .
// . . . . . X X X X X . . . . .     . . . . X X X X X X . . . . .     . . . . . X X X X X . . . . .
// . . . X X . X X . . X X . . .     . . . X X X X . . . X X . . .     . . . X X . . X . . X X . . .
// . . X . X . . X X . . . X . .     . . X . . X X X . X . . X . .     . . X . . X X X . X . . X . .
// . X . . . X X . X . X . . X .     . X . . . . X . . . X . . X .     . X . X . . X . X . . . . X .
// . X X . . . . X . . . . X X .     . X X . . X . X . . . . X X .     . X X . . . . X . X . . X X .
// . X . X X . . . . . X X . X .     . X . X X . . . . . X X . X .     . X . X X . . . . . X X . X .
// . X . . . X X X X X . . . X .     . X . . . X X X X X . . . X .     . X . . . X X X X X . . . X .
// . . X . . . . . . . . . X . .     . . X . . . . . . . . . X . .     . . X . . . . . . . . . X . .
// . . X . . . . . X X X . X . .     . . X . . . . . X X X . X . .     . . X . . . . . X X X . X . .
// . X X X . . . X X . . X X X .     . X X X . . . X X . . X X X .     . X X X . . . X X . . X X X .
// . X . X X . . . . . X X . X .     . X . X X . . . . . X X . X .     . X . X X . . . . . X X . X .
// X X . . X X X X X X X . . X X     X X . . X X X X X X X . . X X     X X . . X X X X X X X . . X X
// X . . . . X . X . X . . . . X     X . . . . X . X . X . . . . X     X . . . . X . X . X . . . . X
// X . . . X . . X . . X . . . X     X . . . X . . X . . X . . . X     X . . . X . . X . . X . . . X
//
// 15x20                             15x20
// . . . . . . . . . . . . . . .     . . . . . . . . . . . . . . .
// . . . . . . . . . . . . . . .     . . . . . . . . . . . . . . .
// . . . . . . . . . . . . . . .     . . . . . . . . . X . . . X .
// . . . . . . . . . . . . . . .     . . . . . . . . . . . . . . .
// . . . . . X . . X . . X . . .     . . . . . . X . . . . X . . .
// . . . . . . . X . . . . . . .     . . . . . . . . . . X . . . .
// . . . . . X X X X X . . . . .     . . . . . X X X X X X . X . .
// . . . X X . . X . . X X . . .     . . . X X . X . X X X X . . .
// . . X . . . . X X X . . X . .     . . X . . . . X X X . . X . .
// . X . . X . X . X . X . . X .     . X . . X X . . X . . X . X .
// . X X . . . . X . . . . X X .     . X X . . . X X . . . . X X .
// . X . X X . . . . . X X . X .     . X . X X . . . . . X X . X .
// . X . . . X X X X X . . . X .     . X . . . X X X X X . . . X .
// . . X . . . . . . . . . X . .     . . X . . . . . . . . . X . .
// . . X . . . . . X X X . X . .     . . X . . . . . X X X . X . .
// . X X X . . . X X . . X X X .     . X X X . . . X X . . X X X .
// . X . X X . . . . . X X . X .     . X . X X . . . . . X X . X .
// X X . . X X X X X X X . . X X     X X . . X X X X X X X . . X X
// X . . . . X . X . X . . . . X     X . . . . X . X . X . . . . X
// X . . . X . . X . . X . . . X     X . . . X . . X . . X . . . X
#define CAULDRON_BMP_FRAME_SIZE 40U
#define CAULDRON_BMP_FRAME_COUNT 5U
#define CAULDRON_BMP_FRAME_WIDTH 15U
#define CAULDRON_BMP_FRAME_HEIGHT 20U
extern const uint8_t cauldron_bmp[];

// 13x12
// . . . . . X X X X . . . .
// . . . . X X X X . X . . .
// . . . X X . . . X X X X .
// . . X . X . . . . . . X X
// X X X . . . . . . . . . X
// X . . . . . . . . . . . X
// X X . . . X X . . . X X X
// . X . . X X X . . . X X .
// . X . . . . . . . . . X .
// . X X X X X . . . . X X .
// . . . . . X X . . X . . .
// . . . . . . . X X . . . .
#define ASTEROID_BIG_BMP_FRAME_SIZE 24U
#define ASTEROID_BIG_BMP_FRAME_COUNT 1U
#define ASTEROID_BIG_BMP_FRAME_WIDTH 13U
#define ASTEROID_BIG_BMP_FRAME_HEIGHT 12U
extern const uint8_t asteroid_big_bmp[];

// 7x6
// . . X X X X .
// . X X . . . X
// X . . X . X X
// X . . . X X .
// . X . . . X .
// . . X X X . .
#define ASTEROID_SMALL_BMP_FRAME_SIZE 6U
#define ASTEROID_SMALL_BMP_FRAME_COUNT 1U
#define ASTEROID_SMALL_BMP_FRAME_WIDTH 7U
#define ASTEROID_SMALL_BMP_FRAME_HEIGHT 6U
extern const uint8_t asteroid_small_bmp[];

#define GLYPH_BMP_FRAME_SIZE 8U
#define GLYPH_BMP_FRAME_COUNT 2U
#define GLYPH_BMP_FRAME_WIDTH 8U
#define GLYPH_BMP_FRAME_HEIGHT 8U

// 8x8               8x8
// . X . . . . X .   . . X . . X . .
// . . X . . X . .   . . X . . X . .
// . X X X X X X .   . X X X X X X .
// . X . X X . X .   . X . X X . X .
// X X X X X X X X   X X X . . X X X
// X . X . . X . X   X . X . . X . X
// . . X X X X . .   . . X X X X . .
// . . X . . X . .   . . X . . X . .
extern const uint8_t alien_glyph_bmp[];

// 8x8               8x8
// . . X X X . . .   . . X X X . . .
// . X . . . X . .   . X X X X X . .
// X . X . X . X .   X X . X . X X .
// X . X . X . X .   X X . X . X X .
// X . . . . . X .   X X X X X X X .
// X . . . . . X .   X X X X X X X .
// X X . X . X X .   X X X X X X X .
// X . X . X . X .   X . X . X . X .
extern const uint8_t ghost_glyph_bmp[];

// 8x8               8x8
// X . . . . . . X   . . . . . . . .
// X X . . . . X X   X X . . . . X X
// X . X X X X . X   X . X X X X . X
// X X X X X X X X   X X X X X X X X
// X . . X X . . X   X x x X X x x X
// X X X X X X X X   X X X X X X X X
// . X X X X X X .   . X X X X X X .
// X . X X X X . X   . . X X X X . .
extern const uint8_t cat_glyph_bmp[];

// 8x8               8x8
// X . . . . . . X   X . . . . . . X
// X . . . . . . X   X . . . . . . X
// X X X X X X X X   X X X X X X X X
// . X . X X . X .   . X . X X . X .
// X X . X X . X X   X X X X X X X X
// X X X X X X X X   X X . . . . X X
// . X . . . . X .   . X . . . . X .
// . X X X X X X .   . X X X X X X .
extern const uint8_t demon_glyph_bmp[];
