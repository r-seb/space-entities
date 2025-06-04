#include "uart.h"
#include "TM4C123GH6PM.h"
#include "active_object.h"
#include "app.h"
#include "assert_handler.h"
#include "printf/printf.h"
#include "tx_api.h"
#include <stdint.h>
#include <string.h>

void uart_init(uint32_t baud_rate)
{
    // 14.4 Initialization and Configuration, pg 902
    // To enable and initialize the UART, the following steps are necessary:

    /*
     * 1. Enable the UART module using the RCGCUART register (see page 344).
     */
    SYSCTL->RCGCUART |= 1U << 0; // UART0

    /*
     * 2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register (see page 340).
     * To find out which GPIO port to enable, refer to Table 23-5 on page 1351.
     */
    SYSCTL->RCGCGPIO |= 1U << 0;  // GPIOA
    SYSCTL->GPIOHBCTL |= 1U << 0; // Enable High-Performance Bus Control

    /*
     * 3. Set the GPIO AFSEL bits for the appropriate pins (see page 671). To determine which GPIOs
     * to configure, see Table 23-4 on page 1344.
     */
    GPIOA_AHB->DEN |= U0RX | U0TX;
    GPIOA_AHB->AFSEL |= U0RX | U0TX;

    /*
     * 4. Configure the GPIO current level and/or slew rate as specified for the mode selected (see
     * page 673 and page 681).
     */
    // Leave this at default

    /*
     * 5. Configure the PMCn fields in the GPIOPCTL register to assign the UART signals to the
     * appropriate pins (see page 688 and Table 23-5 on page 1351).
     */
    GPIOA_AHB->PCTL |= (1U << (U0RX * 4)) | (1U << (U0TX * 4));

    // Configure baud rate, pg. 903
    // BRD = BRDI + BRDF = UARTSysClk / (ClkDiv * Baud Rate), pg. 896
    // Ex for UARTSyClk = 20MHz, ClkDiv = 16 (See UARTCTL HSE bit), Baud Rate = 115200
    // BRD = 20,000,000 / (16 * 115,200) = 10.8507
    // UARTIBRD = 10;
    // UARTFBRD[DIVFRAC] = integer(0.8507 * 64 + 0.5) = 54
    uint32_t ClkDiv_x_BaudRate = (16 * baud_rate);

    /*
     * 1. Disable the UART by clearing the UARTEN bit in the UARTCTL register.
     */
    UART0->CTL &= ~(1U << 0);

    /*
     * 2. Write the integer portion of the BRD to the UARTIBRD register.
     */
    UART0->IBRD = SystemCoreClock / ClkDiv_x_BaudRate;

    /*
     * 3. Write the fractional portion of the BRD to the UARTFBRD register.
     * From the example calculation above, the BRD in terms of fraction is:
     *     1568000  (remainder)
     * 10 ---------
     *     1843200  (ClkDiv * Baud_Rate)
     * So we can approximate using the following formula:
     */
    uint32_t remainder = SystemCoreClock % ClkDiv_x_BaudRate;
    UART0->FBRD = (remainder * 64) / ClkDiv_x_BaudRate;

    /*
     * 4. Write the desired serial parameters to the UARTLCRH register (in this case, a value of
     * 0x0000.0060).
     */
    UART0->LCRH = (3U << 5) | (1U << 4); // Enable 8-bit frame, FIFO

    // Configure interrupt, see 14.3.9 Interrupts, pg. 900
    UART0->IM = (1U << 4) | (1U << 10); // Enable RXIM, OEIM

    /*
     * 5. Configure the UART clock source by writing to the UARTCC register.
     */
    // Leave at default

    /*
     * 6. Optionally, configure the µDMA channel (see “Micro Direct Memory Access (μDMA)” on page
     * 585) and enable the DMA option(s) in the UARTDMACTL register.
     */
    // Not used yet

    /*
     * 7. Enable the UART by setting the UARTEN bit in the UARTCTL register.
     */
    UART0->CTL = (1U << 0) | (1U << 8) | (1U << 9); // Enable UARTEN, TXE, RXE

    NVIC_SetPriority(UART0_IRQn, 7);
    NVIC_EnableIRQ(UART0_IRQn);
}

void UART0_IRQHandler()
{
    // See pg. 930
    uint32_t isrs = UART0->MIS;
    UART0->ICR = 0xFFU; // Clear interrupts

    // OEMIS
    if (isrs & (1U << 10)) {
        // TODO: Overrun error check
    }
    // RXMIS
    else if (isrs & (1U << 4)) {
        // TODO: Receive operation
    }
}

int uart_send(const char* format, ...)
{
    static char buffer[UART_MSG_SIZE]; // Character limit
    va_list args;
    va_start(args, format);
    const int buf_size = vsnprintf_(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Flexible array members
    // https://developer.arm.com/documentation/dui0375/g/Compiler-Coding-Practices/Flexible-array-members-in-C99
    uint32_t total_size = sizeof(SerialEvent) + buf_size + 1;
    SerialEvent* serial_evt;
    // NOTE: This call is not supported by an interrupt service routine, it returns TX_CALLER_ERROR
    UINT status = EVENT_ALLOCATE_WITH_SIZE(uart_evt_byte_pool, serial_evt, total_size);
    if (status == TX_SUCCESS) {
        serial_evt->super.sig = UART_SEND_SIG;
        // NOTE: String formatted using vsnprintf_ is automatically null-terminated so we also
        // include it, hence the +1
        serial_evt->buffer_size = buf_size + 1;
        memcpy(serial_evt->buffer, buffer, buf_size + 1);

        Active_post_nonthread(AO_UARTManager, (Event*)serial_evt);
        return buf_size;
    }
    // TODO: Find a way to indicate that the pool allocation was a failure
    return status;
}

void uart_fill_fifo(char** c)
{
    ASSERT(c && *c);

    // NOTE: SerialEvents are always null-terminated
    while (!(UART0->FR & (1U << 5)) && (**c != '\0')) {
        UART0->DR = **c;
        (*c)++;
    }
    static Event const uart_tx_sig = {UART_TRANSMITTED_SIG};
    Active_post(AO_UARTManager, &uart_tx_sig);
}

// To avoid undefined reference since printf.c expects an implementation
void putchar_(char c)
{
    (void)c;
}
