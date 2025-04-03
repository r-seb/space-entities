#include "i2c.h"
#include "TM4C123GH6PM.h"
#include "printf.h"
#include <stdint.h>

_Noreturn extern void assert_failed(char const* const module, int const id);

void i2c1_init(uint32_t i2c_speed)
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
    I2C1->MTPR = (((SystemCoreClock / (2 * (6 + 4) * i2c_speed)) - 1) << 0);
}

static i2c_status_e i2c1_wait_tx_rx(const char* str)
{
    while ((I2C1->MCS & I2C_STATUS_BUSY)) {}
    i2c_status_e stat = I2C1->MCS;
    if (stat & I2C_STATUS_ERROR) {
        printf_("I2C %s Error\n\r", str);
        return stat;
    }
    return I2C_STATUS_OK;
}

i2c_status_e i2c1_write(uint8_t slave_addr, uint8_t* buffer, uint8_t buf_size)
{
    // TODO: Add assertions

    /*
     * 8. Specify the slave address of the master and that the next operation is a Transmit by
     * writing the I2CMSA register with a value of 0x0000.0076. This sets the slave address to 0x3B.
     */
    I2C1->MSA = slave_addr << 1; // I2C Slave Address should be placed at bit 7:1, pg 1019

    /*
     * 9. Place data (byte) to be transmitted in the data register by writing the I2CMDR register
     * with the desired data.
     */
    I2C1->MDR = *buffer++;

    i2c_status_e stat = I2C_STATUS_ERROR;
    // refer pg 1008, Figure 16-8. Master Single TRANSMIT
    if (buf_size == 1) {
        /*
         * 10. Initiate a single byte transmit of the data from Master to Slave by writing the
         * I2CMCS register with a value of 0x0000.0007 (STOP, START, RUN). refer pg 1023
         * refer pg 1024, Table 16-5. Write Field Decoding for I2CMCS[3:0] Field
         */
        I2C1->MCS = (1U << 2) | (1U << 1) | (1U << 0); // STOP, START, RUN

        /*
         * 11. Wait until the transmission completes by polling the I2CMCS register's BUSBSY bit
         * until it has been cleared.
         */
        stat = i2c1_wait_tx_rx("Write");
    }
    // refer pg 1010, Figure 16-10. Master TRANSMIT of Multiple Data Bytes
    else if (buf_size > 1) {
        // Send first data
        I2C1->MCS = (1U << 1) | (1U << 0); // START, RUN
        stat = i2c1_wait_tx_rx("Write");
        if (stat) {
            return stat;
        }

        // Send other data
        while ((buf_size - 1) > 1) {
            I2C1->MDR = *buffer++;
            I2C1->MCS = (1U << 0); // RUN
            stat = i2c1_wait_tx_rx("Write");
            if (stat) {
                return stat;
            }
            buf_size--;
        }

        // Send last data
        I2C1->MDR = *buffer;
        I2C1->MCS = (1U << 2) | (1U << 0); // STOP, RUN
        stat = i2c1_wait_tx_rx("Write");
    }
    return (stat) ? stat : I2C_STATUS_OK;
}

i2c_status_e i2c1_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t* store, uint8_t store_size)
{
    // TODO: Add assertions

    i2c1_write(slave_addr, &reg_addr, 1);

    // Transition to receive operation
    I2C1->MSA = (slave_addr << 1) | (1U << 0);

    i2c_status_e stat = I2C_STATUS_ERROR;
    // refer pg. 1009, Figure 16-9. Master Single RECEIVE
    if (store_size == 1) {
        I2C1->MCS = (1U << 2) | (1U << 1) | (1U << 0); // STOP, START, RUN
        stat = i2c1_wait_tx_rx("Read");
        *store = (stat) ? 0x00 : I2C1->MDR;
        // while ((I2C1->MCS & (1U << 6))) {}
    }
    // refer pg. 1011, Figure 16-11. Master RECEIVE of Multiple Data Bytes
    else if (store_size > 1) {
        // Read first data
        I2C1->MCS = (1U << 3) | (1U << 1) | (1U << 0); // ACK, START, RUN
        stat = i2c1_wait_tx_rx("Read");
        if (stat) {
            return stat;
        }
        *store++ = I2C1->MDR;

        // Read other data
        while ((store_size - 0) > 1) {
            I2C1->MCS = (1U << 3) | (1U << 0); // ACK, RUN
            stat = i2c1_wait_tx_rx("Read");
            if (stat) {
                return stat;
            }
            *store++ = I2C1->MDR;
            store_size--;
        }

        // Read last data
        I2C1->MCS = (1U << 2) | (1U << 0); // STOP, RUN
        stat = i2c1_wait_tx_rx("Read");
        *store = I2C1->MDR;
        // while ((I2C1->MCS & (1U << 6))) {}
    }
    return (stat) ? stat : I2C_STATUS_OK;
}