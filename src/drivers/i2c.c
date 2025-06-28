#include "i2c.h"
#include "TM4C123GH6PM.h"
#include "active_object.h"
#include "app.h"
#include "assert_handler.h"
#include "led.h"
#include "state_machine.h"
#include "tx_api.h"
#include "uart.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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

    // 16.3.3.1 I2C Master Interrupts, pg. 1006
    I2C1->MIMR = (1U << 0); // Set IM
    NVIC_SetPriority(I2C1_IRQn, 6);
    NVIC_EnableIRQ(I2C1_IRQn);
}

static uint8_t* _cur_buffer;
static uint8_t _cur_buffer_size;
static i2c_data* _cur_data;

void I2C1_IRQHandler()
{
    I2C1->MICR = (1U << 0); // Clear IC

    uint32_t mcs = I2C1->MCS;

    static Event i2c_evt;
    i2c_evt.sig = I2C_ERROR_SIG;
    // BUSY
    if ((mcs & I2C_STATUS_BUSY) != 0) {
        led_toggle(LED_RED);
    }
    // ARBLST or ADRACK or DATACK
    else if ((mcs & (I2C_STATUS_ARBLST | I2C_STATUS_ADRACK | I2C_STATUS_DATACK)) != 0) {
        // TODO: Handle errata, I2C#07
        _cur_buffer = NULL;
        Active_post_front(AO_I2CManager, &i2c_evt);
    }
    // Last transaction (transmit/receive) must be ok
    else {
        i2c_evt.sig = I2C_TRANSACTION_OK_SIG;

        uint8_t op = I2C_RUN_OP;
        bool done = false;

        if (_cur_data->mode == I2C_MASTER_RECEIVE) {
            *_cur_buffer++ = I2C1->MDR;

            if (_cur_buffer_size == 0) {
                done = true; // Last byte was just received
            } else {
                op |= I2C_ACK_OP;
            }

        } else if (_cur_data->mode == I2C_MASTER_TRANSMIT) {

            if (_cur_buffer_size == 0) {
                done = true; // Nothing to send
            } else {
                I2C1->MDR = *_cur_buffer++;
            }
        }

        if (done) {
            Active_post_front(AO_I2CManager, &i2c_evt);
            _cur_data = NULL;
            _cur_buffer = NULL;

        } else {
            // Is this the last byte?
            if (_cur_buffer_size == 1) {
                op = (op & ~I2C_ACK_OP) | I2C_STOP_OP;
            }
            _cur_buffer_size--;

            I2C1->MCS = op;
        }
    }
}

void i2c1_set_slave_address(uint8_t addr, master_mode mode)
{
    // I2C Slave Address should be placed at bit 7:1, pg 1019
    // Operation at bit 0
    I2C1->MSA = (addr << 1) | (mode << 0);
}

void i2c1_transmit(i2c_data* data)
{
    ASSERT(data->mode == I2C_MASTER_TRANSMIT);
    _cur_data = data;
    _cur_buffer = data->write_buffer;
    _cur_buffer_size = data->write_buffer_size;

    // Set up to transfer first byte
    i2c1_set_slave_address(data->slave_addr, data->mode);
    uint8_t op = I2C_START_OP | I2C_RUN_OP;
    if (_cur_data->write_buffer_size == 1) {
        op |= I2C_STOP_OP;
    }
    _cur_buffer_size--;
    I2C1->MDR = *_cur_buffer++;
    I2C1->MCS = op;
}

void i2c1_receive(i2c_data* data)
{
    ASSERT(data->mode == I2C_MASTER_RECEIVE);
    _cur_data = data;
    _cur_buffer = data->store_buffer;
    _cur_buffer_size = data->store_buffer_size;

    // Set up to receive first byte
    i2c1_set_slave_address(data->slave_addr, data->mode);
    uint8_t op = I2C_ACK_OP | I2C_START_OP | I2C_RUN_OP;
    if (_cur_buffer_size == 1) {
        op = (op & ~I2C_ACK_OP) | I2C_STOP_OP;
    }
    _cur_buffer_size--;
    I2C1->MCS = op;
}

void i2c1_generate_stop()
{
    I2C1->MCS = I2C_STOP_OP;
}

i2c_status_e i2c1_write(uint8_t slave_addr, uint8_t* buffer, uint8_t buf_size)
{
    // Flexible array members
    // https://developer.arm.com/documentation/dui0375/g/Compiler-Coding-Practices/Flexible-array-members-in-C99
    uint32_t total_size = sizeof(I2CEvent) + buf_size;
    I2CEvent* i2c_evt;
    // NOTE: This call is not supported by an isr, it returns TX_CALLER_ERROR
    UINT status = EVENT_ALLOCATE_WITH_SIZE(msg_evt_byte_pool, i2c_evt, total_size);
    if (status == TX_SUCCESS) {
        i2c_evt->super.sig = I2C_TRANSMIT_START_SIG;
        i2c_evt->data.slave_addr = slave_addr;
        i2c_evt->data.write_buffer_size = buf_size;
        memcpy(i2c_evt->data.write_buffer, buffer, buf_size);

        Active_post_nonthread(AO_I2CManager, (Event*)i2c_evt);
        return I2C_STATUS_OK;
    }
    uart_send("I2Cpool error: 0x%X\n\r", status);
    return I2C_STATUS_ERROR;
}

i2c_status_e i2c1_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t* store, uint8_t store_size,
                       Active* requester)
{
    // Flexible array members
    // https://developer.arm.com/documentation/dui0375/g/Compiler-Coding-Practices/Flexible-array-members-in-C99
    uint32_t total_size = sizeof(I2CEvent) + 1;
    I2CEvent* i2c_evt;
    // NOTE: This call is not supported by an isr, it returns TX_CALLER_ERROR
    UINT status = EVENT_ALLOCATE_WITH_SIZE(msg_evt_byte_pool, i2c_evt, total_size);
    if (status == TX_SUCCESS) {
        i2c_evt->data.requester = requester;
        i2c_evt->super.sig = I2C_RECEIVE_START_SIG;
        i2c_evt->data.slave_addr = slave_addr;
        i2c_evt->data.store_buffer_size = store_size;
        i2c_evt->data.write_buffer_size = 1;
        i2c_evt->data.store_buffer = store;
        i2c_evt->data.write_buffer[0] = reg_addr;

        Active_post_nonthread(AO_I2CManager, (Event*)i2c_evt);
        return I2C_STATUS_OK;
    }
    uart_send("I2Cpool error: 0x%X\n\r", status);
    return I2C_STATUS_ERROR;
}
