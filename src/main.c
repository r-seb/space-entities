#include "TM4C123GH6PM.h"
#include "assert_handler.h"
#include "assets.h"
#include "delay.h"
#include "i2c.h"
#include "led.h"
#include "mpu6050_i2c.h"
#include "printf.h"
#include "ssd1309_128x64_i2c.h"
#include "tx_api.h"
#include "u8g2.h"
#include "uart.h"
#include <stdbool.h>
#include <stdint.h>

TX_THREAD blinky_thread;

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

    /* Enter the ThreadX kernel. */
    // tx_kernel_enter();
    // The function below should never execute

    // Ship should spawn in the middle of screen
    uint8_t x = 128 / 2 - 8;
    uint8_t y = 64 / 2 - 8;
    mpu6050_data imu;
    float move_thres = 0.02f; // acc approx value between -1g to 1g
    while (1) {
        // Check for imu readings
        if (mpu6050_is_data_ready()) {
            mpu6050_read_data(&imu);
        }

        // Clear buffer and draw the player ship
        u8g2_ClearBuffer(&oled);
        u8g2_DrawXBM(&oled, x, y, 8, 8, player_ship1_bmp);
        u8g2_SendBuffer(&oled);

        // UP-DOWN Movement
        if (imu.accx > move_thres) {
            if (y > 0) {
                y--;
            } else {
                y = 0;
            }
        } else if (imu.accx < -move_thres) {
            if (y < 56) {
                y++;
            } else {
                y = 56;
            }
        }
        // LEFT-RIGHT Movement
        if (imu.accy > move_thres) {
            if (x > 0) {
                x--;
            } else {
                x = 0;
            }
        } else if (imu.accy < -move_thres) {
            if (x < 118) {
                x++;
            } else {
                x = 118;
            }
        }
    }

    return 0;
}

void blinky_thread_entry(ULONG thread_input)
{
    /* Enter into a forever loop. */
    while (1) {
        /* Blink LED */
        led_toggle(LED_BLUE);
        /* Sleep for 1000 tick. */
        tx_thread_sleep(1000);
    }
}

void tx_application_define(void* first_unused_memory)
{
    /* Create blinky_thread! */
    tx_thread_create(&blinky_thread, "Blinky Thread", blinky_thread_entry, 0x1234,
                     first_unused_memory, 1024, 3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
}
