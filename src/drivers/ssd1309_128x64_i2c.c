#include "ssd1309_128x64_i2c.h"
#include "i2c.h"
#include "u8g2.h"

// https://github.com/olikraus/u8g2/wiki/Porting-to-new-MCU-platform

static i2c_write_handler i2c_write;

void ssd1309_128x64_init(u8g2_t* oled, i2c_write_handler write_handler)
{
    i2c_write = write_handler;
    u8g2_Setup_ssd1309_i2c_128x64_noname0_f(oled, U8G2_R0, u8x8_i2c, u8x8_gpio_and_delay_i2c);
    u8g2_InitDisplay(oled);
    u8g2_SetPowerSave(oled, 0);
    u8g2_SetFont(oled, u8g2_font_siji_t_6x10);
    u8g2_SetBitmapMode(oled, 1); // Make bitmap transparent
    u8g2_SetContrast(oled, 0);
    u8g2_ClearDisplay(oled);
}

uint8_t u8x8_gpio_and_delay_i2c(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr)
{
    switch (msg) {
        case U8X8_MSG_DELAY_MILLI:
            // delay_ms(arg_int);
            break;
    }
    return 1;
}

uint8_t u8x8_i2c(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr)
{
    static uint8_t buffer[32]; /* u8g2/u8x8 will never send more than 32 bytes between
                                  START_TRANSFER and END_TRANSFER */
    static uint8_t buf_idx;
    uint8_t* data;

    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t*)arg_ptr;
            while (arg_int > 0) {
                buffer[buf_idx++] = *data;
                data++;
                arg_int--;
            }
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            (*i2c_write)(u8x8_GetI2CAddress(u8x8) >> 1, buffer, buf_idx);
            break;
        default:
            return 0;
    }
    return 1;
}
