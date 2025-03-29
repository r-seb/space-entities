#include "TM4C123GH6PM.h"
#include "delay.h"
#include "led.h"
#include "assets.h"
#include "ssd1309_128x64_i2c.h"
#include "u8g2.h"
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

    u8g2_DrawXBM(&oled, 56, 24, 16, 16, boss_ship_bmp);
    u8g2_DrawXBM(&oled, 56, 45, 8, 8, player_ship_bmp);
    u8g2_SendBuffer(&oled);

    while (1) {
        led_toggle(LED_BLUE);
        delay_ms(1000);
    }

    return 0;
}
