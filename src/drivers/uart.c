#include "uart.h"
#include "TM4C123GH6PM.h"
#include <stdint.h>

void uart_init(void)
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
    uint32_t ClkDiv_x_BaudRate = (16 * UART_BAUD_RATE);

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
    UART0->LCRH = 0x3U << 5; // 8-bit frame

    /*
     * 5. Configure the UART clock source by writing to the UARTCC register.
     */
    // Leave at default

    /*
     * 6. Optionally, configure the µDMA channel (see “Micro Direct Memory Access (μDMA)” on page
     * 585) and enable the DMA option(s) in the UARTDMACTL register.
     */
    // Not used

    /*
     * 7. Enable the UART by setting the UARTEN bit in the UARTCTL register.
     */
    UART0->CTL = (1U << 0) | (1U << 8) | (1U << 9); // Enable UARTEN, TXE, RXE
}

void putchar_(char c)
{
    // FIFO Operation, pg. 900
    while (UART0->FR & (1U << 3)) {} // Check BUSY bit
    UART0->DR = c;
}