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
const uint8_t player_ship_bmp[] = {
    // Frame 0
    0xf0, 0x00, 0x24, 0x03, 0x78, 0x00, 0x8c, 0x00, 0x8c, 0x00, 0x78, 0x00, 0x24, 0x03, 0xf0, 0x00,
    // Frame 1
    0xf0, 0x00, 0x24, 0x03, 0x7a, 0x00, 0x8e, 0x00, 0x8e, 0x00, 0x7a, 0x00, 0x24, 0x03, 0xf0, 0x00,
    // Frame 2
    0xf0, 0x00, 0x24, 0x03, 0x78, 0x00, 0x8d, 0x00, 0x8d, 0x00, 0x78, 0x00, 0x24, 0x03, 0xf0, 0x00};

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
const uint8_t player_ship2_bmp[] = {0x70, 0x00, 0xf8, 0x07, 0x7c, 0x0e, 0x3f, 0x00, 0x7e,
                                    0x00, 0xc5, 0x00, 0x07, 0x01, 0xc5, 0x00, 0x7e, 0x00,
                                    0x3f, 0x00, 0x7c, 0x0e, 0xf8, 0x07, 0x70, 0x00};

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
const uint8_t boss_ship_bmp[] = {0x00, 0x7f, 0x00, 0x18, 0x00, 0x18, 0x80, 0x7f, 0x00, 0x3f, 0xf0,
                                 0x1f, 0x1c, 0xcf, 0x0f, 0x7e, 0x0f, 0x7e, 0x1c, 0xcf, 0xf0, 0x1f,
                                 0x00, 0x3f, 0x80, 0x7f, 0x00, 0x18, 0x00, 0x18, 0x00, 0x7f};

// 7x7
// . . . X . . .
// . X . . . X .
// . . X X X . .
// X . X X X . X
// . . X X X . .
// . X . . . X .
// . . . X . . .
#define MINE_BMP_FRAME_SIZE 7U
#define MINE_BMP_FRAME_COUNT 1U
#define MINE_BMP_FRAME_WIDTH 7U
#define MINE_BMP_FRAME_HEIGHT 7U
const uint8_t mine_bmp[] = {0x10, 0x44, 0x38, 0xba, 0x38, 0x44, 0x10};

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
const uint8_t crawler_bmp[] = {
    // Frame 0
    0x20, 0x00, 0x20, 0x00, 0x76, 0x00, 0x54, 0x06, 0x74, 0x02, 0x04, 0x02, 0xfc, 0x03, 0xdc, 0x01,
    0x8c, 0x01, 0xdc, 0x01, 0xfe, 0x01, 0x22, 0x01, 0x02, 0x01, 0x03, 0x01, 0x00, 0x03,
    // Frame 1
    0x50, 0x00, 0x20, 0x00, 0x70, 0x03, 0x53, 0x01, 0x72, 0x01, 0x02, 0x01, 0xfe, 0x01, 0xac, 0x01,
    0xdc, 0x01, 0xac, 0x01, 0xfc, 0x03, 0x24, 0x02, 0x04, 0x02, 0x04, 0x06, 0x06, 0x00};

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
const uint8_t cauldron_bmp[] = {
    // Frame 0
    0x40, 0x00, 0x40, 0x02, 0xa0, 0x00, 0x48, 0x01, 0xc4, 0x00, 0x90, 0x01, 0xe0, 0x03, 0xd8, 0x0c,
    0x94, 0x11, 0x62, 0x25, 0x86, 0x30, 0x1a, 0x2c, 0xe2, 0x23, 0x04, 0x10, 0x04, 0x17, 0x8e, 0x39,
    0x1a, 0x2c, 0xf3, 0x67, 0xa1, 0x42, 0x91, 0x44,
    // Frame 1
    0x00, 0x00, 0x08, 0x00, 0x32, 0x00, 0x10, 0x00, 0xc8, 0x00, 0x12, 0x00, 0xf0, 0x03, 0x78, 0x0c,
    0xe4, 0x12, 0x42, 0x24, 0xa6, 0x30, 0x1a, 0x2c, 0xe2, 0x23, 0x04, 0x10, 0x04, 0x17, 0x8e, 0x39,
    0x1a, 0x2c, 0xf3, 0x67, 0xa1, 0x42, 0x91, 0x44,
    // Frame 2
    0x00, 0x00, 0x00, 0x02, 0x10, 0x01, 0x80, 0x00, 0x10, 0x01, 0x80, 0x00, 0xe0, 0x03, 0x98, 0x0c,
    0xe4, 0x12, 0x4a, 0x21, 0x86, 0x32, 0x1a, 0x2c, 0xe2, 0x23, 0x04, 0x10, 0x04, 0x17, 0x8e, 0x39,
    0x1a, 0x2c, 0xf3, 0x67, 0xa1, 0x42, 0x91, 0x44,
    // Frame 3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x09, 0x80, 0x00, 0xe0, 0x03, 0x98, 0x0c,
    0x84, 0x13, 0x52, 0x25, 0x86, 0x38, 0x1a, 0x2c, 0xe2, 0x23, 0x04, 0x10, 0x04, 0x17, 0x8e, 0x39,
    0x1a, 0x2c, 0xf3, 0x67, 0xa1, 0x42, 0x91, 0x44,
    // Frame 4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x40, 0x08, 0x00, 0x04, 0xe0, 0x17, 0x58, 0x0f,
    0x84, 0x13, 0x32, 0x29, 0xc6, 0x30, 0x1a, 0x2c, 0xe2, 0x23, 0x04, 0x10, 0x04, 0x17, 0x8e, 0x39,
    0x1a, 0x2c, 0xf3, 0x67, 0xa1, 0x42, 0x91, 0x44};

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
const uint8_t asteroid_big_bmp[] = {0xe0, 0x01, 0xf0, 0x02, 0x18, 0x0f, 0x14, 0x18,
                                    0x07, 0x10, 0x01, 0x10, 0x63, 0x1c, 0x72, 0x0c,
                                    0x02, 0x08, 0x3e, 0x0c, 0x60, 0x02, 0x80, 0x01};

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
const uint8_t asteroid_small_bmp[] = {0x3c, 0x46, 0x69, 0x31, 0x22, 0x1c};
