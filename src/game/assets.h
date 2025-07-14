#pragma once

#include <stdint.h>

// 8x8
// . . X X X X . .
// X . . X . . X X
// . X X X X . . .
// X X . . . X . .
// X X . . . X . .
// . X X X X . . .
// X . . X . . X X
// . . X X X X . .
const uint8_t player_ship1_bmp[] = {0x3c, 0xc9, 0x1e, 0x23, 0x23, 0x1e, 0xc9, 0x3c};

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