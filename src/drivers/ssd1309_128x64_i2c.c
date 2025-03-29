#include "ssd1309_128x64_i2c.h"
#include "TM4C123GH6PM.h"
#include "delay.h"
#include "u8g2.h"

// https://github.com/olikraus/u8g2/wiki/Porting-to-new-MCU-platform

void ssd1309_128x64_init(u8g2_t* oled)
{
    u8g2_Setup_ssd1309_i2c_128x64_noname2_f(oled, U8G2_R0, u8x8_i2c, u8x8_gpio_and_delay_i2c);
    u8g2_InitDisplay(oled);
    u8g2_SetPowerSave(oled, 0);
    u8g2_ClearDisplay(oled);
}

uint8_t u8x8_gpio_and_delay_i2c(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr)
{
    switch (msg) {
        case U8X8_MSG_DELAY_MILLI:
            delay_ms(arg_int);
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
        case U8X8_MSG_BYTE_INIT:
            i2c1_init();
            break;
        case U8X8_MSG_BYTE_SET_DC:
            /* ignored for i2c */
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            i2c1_write(u8x8_GetI2CAddress(u8x8) >> 1, buffer, buf_idx);
            break;
        default:
            return 0;
    }
    return 1;
}

void i2c1_init()
{
    // Refer Table 16-1. I2C Signals (64LQFP), [pg. 998]
    // 16.4.1 Configure the I2C Module to Transmit a Single Byte as a Master, [pg. 1015]

    /*
     * 1. Enable the I2C clock using the RCGCI2C register in the System Control module (see page
     * 348).
     */
    SYSCTL->RCGCI2C = (1U << 1); // I2C Module 1

    /*
     * 2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register in the System
     * Control module (see page 340). To find out which GPIO port to enable, refer to Table
     * 23-5 on page 1351.
     */
    SYSCTL->RCGCGPIO |= (1U << 0);  // GPIOA
    SYSCTL->GPIOHBCTL |= (1U << 0); // Enable High-Performance Bus Control

    /*
     * 3. In the GPIO module, enable the appropriate pins for their alternate function using the
     * GPIOAFSEL register (see page 671).
     */
    GPIOA_AHB->DEN |= SCL_PIN | SDA_PIN;
    GPIOA_AHB->AFSEL |= SCL_PIN | SDA_PIN;

    /*
     * 4. Enable the I2CSDA pin for open-drain operation. See page 676.
     */
    GPIOA_AHB->ODR |= SDA_PIN;

    /*
     * 5. Configure the PMCn fields in the GPIOPCTL register to assign the I2C signals to the
     * appropriate pins. See page 688 and Table 23-5 on page 1351.
     */
    GPIOA_AHB->PCTL |= (3U << 24) | (3U << 28);

    /*
     * 6. Initialize the I2C Master by writing the I2CMCR register with a value of 0x0000.0010.
     */
    I2C1->MCR = (1U << 4);

    /*
     * 7. Set the desired SCL clock speed of 100 Kbps by writing the I2CMTPR register with the
     * correct value. The value written to the I2CMTPR register represents the number of system
     * clock periods in one SCL clock period.
     * TPR = (System Clock/(2*(SCL_LP + SCL_HP)*SCL_CLK))-1;
     * TPR = (20MHz/(2*(6+4)*100000))-1;
     * TPR = 9
     */
    I2C1->MTPR = (((SystemCoreClock / (2 * (6 + 4) * 1000000)) - 1) << 0);
}

void i2c1_write(uint8_t slave_addr, uint8_t* buffer, uint8_t buf_idx)
{
    /*
     * 8. Specify the slave address of the master and that the next operation is a Transmit by
     * writing the I2CMSA register with a value of 0x0000.0076. This sets the slave address to 0x3B.
     */
    I2C1->MSA |= slave_addr << 1; // I2C Slave Address should be placed at bit 7:1, pg 1019

    /*
     * 9. Place data (byte) to be transmitted in the data register by writing the I2CMDR register
     * with the desired data.
     */
    I2C1->MDR = *buffer++;

    // refer pg 1008, Figure 16-8. Master Single TRANSMIT
    if (buf_idx == 1) {
        /*
         * 10. Initiate a single byte transmit of the data from Master to Slave by writing the
         * I2CMCS register with a value of 0x0000.0007 (STOP, START, RUN). refer pg 1023
         * refer pg 1024, Table 16-5. Write Field Decoding for I2CMCS[3:0] Field
         */
        I2C1->MCS |= (1U << 2) | (1U << 1) | (1U << 0); // STOP, START, RUN

        /*
         * 11. Wait until the transmission completes by polling the I2CMCS register's BUSBSY bit
         * until it has been cleared.
         * Here we poll the BUSY bit because we only have a single master
         */
        while ((I2C1->MCS & (1U << 0))) {}
    }
    // refer pg 1010, Figure 16-10. Master TRANSMIT of Multiple Data Bytes
    else {
        // Send first data
        I2C1->MCS |= (1U << 1) | (1U << 0); // START, RUN
        while ((I2C1->MCS & (1U << 0))) {}

        // Send other data
        while ((buf_idx - 1) > 1) {
            I2C1->MDR = *buffer++;
            I2C1->MCS |= (1U << 0); // RUN
            while (I2C1->MCS & (1U << 0)) {}
            buf_idx--;
        }

        // Send last data
        I2C1->MDR = *buffer;
        I2C1->MCS |= (1U << 2) | (1U << 0); // STOP, RUN
        while ((I2C1->MCS & (1U << 0))) {}
    }
    // TODO: Handle errors
}