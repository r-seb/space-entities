#include "TM4C123GH6PM.h"
#include "assets.h"
#include "delay.h"
#include "i2c.h"
#include "led.h"
#include "mpu6050_i2c.h"
#include "printf.h"
#include "ssd1309_128x64_i2c.h"
#include "u8g2.h"
#include "uart.h"
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
    i2c1_init(I2C_1Mbps);
    uart_init();
    ssd1309_128x64_init(&oled, &i2c1_write);
    mpu6050_init(&i2c1_write, &i2c1_read);
    printf_("Hello from Tiva!!\n\r");

    uint8_t y = 0;
    bool limit_hit_y = false;
    uint8_t x = 0;
    bool limit_hit_x = false;
    while (1) {
        // Clear buffer and draw the player ship
        u8g2_ClearBuffer(&oled);
        u8g2_DrawXBM(&oled, x, y, 8, 8, player_ship1_bmp);
        u8g2_SendBuffer(&oled);

        if (limit_hit_y) {
            if (--y == 0) {
                limit_hit_y = false;
            }
        } else {
            if (++y == 56) {
                limit_hit_y = true;
            }
        }
        if (limit_hit_x) {
            if (--x == 0) {
                limit_hit_x = false;
            }
        } else {
            if (++x == 118) {
                limit_hit_x = true;
            }
        }
    }

    return 0;
}
