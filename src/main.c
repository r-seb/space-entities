#include "TM4C123GH6PM.h"
#include "assets.h"
#include "delay.h"
#include "led.h"
#include "ssd1309_128x64_i2c.h"
#include "u8g2.h"
#include <stdbool.h>
#include <stdint.h>


_Noreturn void assert_failed(char const* const module, int const id)
{
    (void)module; // unused parameter
    (void)id;     // unused parameter
#ifndef NDEBUG
    // light up all LEDs
    GPIOF_AHB->DATA_Bits[LED_GREEN | LED_RED | LED_BLUE] = 0xFFU;
    // for debugging, hang on in an endless loop...
    for (;;) {}
#endif
    NVIC_SystemReset();
}

u8g2_t oled;

int main()
{
    led_init();
    delay_init();
    ssd1309_128x64_init(&oled);

    uint8_t y = 0;
    bool limit_hit = false;
    while (1) {
        if (limit_hit) {
            y--;
            if (y == 0) {
                limit_hit = false;
            }
        } else if (y >= 0 && y <= 56) {
            y++;
            if (y == 56) {
                limit_hit = true;
            }
        }
        u8g2_ClearBuffer(&oled);
        u8g2_DrawXBM(&oled, 60, y, 8, 8, player_ship_bmp);
        u8g2_SendBuffer(&oled);
    }

    return 0;
}
