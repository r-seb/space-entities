#pragma once

#include "TM4C123GH6PM.h" /* the TM4C MCU Peripheral Access Layer (TI) */
#include <stdint.h>

#define LED_RED (1U << 1)
#define LED_BLUE (1U << 2)
#define LED_GREEN (1U << 3)

void led_init(void);

inline void led_on(uint8_t color) { GPIOF_AHB->DATA_Bits[color] = color; }
inline void led_off(uint8_t color) { GPIOF_AHB->DATA_Bits[color] = ~color; }
inline void led_toggle(uint8_t color) { GPIOF_AHB->DATA_Bits[color] ^= color; }