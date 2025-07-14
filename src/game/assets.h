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
// FRAME_SIZE = 16
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
// FRAME_SIZE = 30
const uint8_t crawler_bmp[] = {
    // Frame 0
    0x20, 0x00, 0x20, 0x00, 0x76, 0x00, 0x54, 0x06, 0x74, 0x02, 0x04, 0x02, 0xfc, 0x03, 0xdc, 0x01,
    0x8c, 0x01, 0xdc, 0x01, 0xfe, 0x01, 0x22, 0x01, 0x02, 0x01, 0x03, 0x01, 0x00, 0x03,
    // Frame 1
    0x50, 0x00, 0x20, 0x00, 0x70, 0x03, 0x53, 0x01, 0x72, 0x01, 0x02, 0x01, 0xfe, 0x01, 0xac, 0x01,
    0xdc, 0x01, 0xac, 0x01, 0xfc, 0x03, 0x24, 0x02, 0x04, 0x02, 0x04, 0x06, 0x06, 0x00};

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
const uint8_t asteroid_small_bmp[] = {0x3c, 0x46, 0x69, 0x31, 0x22, 0x1c};
