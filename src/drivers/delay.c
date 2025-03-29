#include "delay.h"
#include "TM4C123GH6PM.h"
#include "led.h"
#include <stdint.h>

void delay_init(void)
{
    /*
     * pg. 722
     * 11.4 Initialization and Configuration
     * The appropriate TIMERn bit must be set in the RCGCTIMER
     */
    SYSCTL->RCGCTIMER |= (1U << 0); // TIMER0 Enable

    /*
     * 11.4.1 One-Shot/Periodic Timer Mode
     * 1. Ensure the timer is disabled (the TnEN bit in the GPTMCTL register is cleared) before
     * making any changes. (refer pg. 737)
     */
    TIMER0->CTL &= ~(1U << 0);

    /*
     * 2. Write the GPTM Configuration Register (GPTMCFG) with a value of 0x0000.0000.
     * (refer pg. 727)
     */
    TIMER0->CFG = 0x00000000U;

    /*
     * 3. Configure the TnMR field in the GPTM Timer n Mode Register (GPTMTnMR):
     * a. Write a value of 0x1 for One-Shot mode.
     * b. Write a value of 0x2 for Periodic mode.
     */
    TIMER0->TAMR = 0x02U;

    /*
     * 4. Optionally configure the TnSNAPS, TnWOT, TnMTE, and TnCDIR bits in the GPTMTnMR register
     * to select whether to capture the value of the free-running timer at time-out, use an external
     * trigger to start counting, configure an additional trigger or interrupt, and count up or
     * down.
     */
    // (refer pg. 729)

    /*
     * 5. Load the start value into the GPTM Timer n Interval Load Register (GPTMTnILR).
     * (refer pg. 756)
     */
    TIMER0->TAILR = (SystemCoreClock / 1000); // To get milliseconds

    /*
     * 6. If interrupts are required, set the appropriate bits in the GPTM Interrupt Mask Register
     * (GPTMIMR). (refer pg. 745)
     * Check the interrupt handler name in tm4c123gh6pm_startup_ccs_gcc.c
     * Enable
     */
    // TIMER0->IMR |= (1U << 0);
    // NVIC->ISER[0] |= (1U << 19);
}

void delay_ms(uint16_t ms)
{
    /*
     * 7. Set the TnEN bit in the GPTMCTL register to enable the timer and start counting.
     * (refer pg. 737)
     */
    TIMER0->CTL |= (1U << 0);

    /*
     * 8. Poll the GPTMRIS register or wait for the interrupt to be generated (if enabled). In both
     * cases, the status flags are cleared by writing a 1 to the appropriate bit of the GPTM
     * Interrupt Clear Register (GPTMICR).
     * (refer pg. 750 & 754)
     */
    while (ms--) {
        while (!(TIMER0->RIS & (1U << 0))) {}
        TIMER0->ICR |= (1U << 0);
    }
    TIMER0->CTL &= ~(1U << 0);
}

void Timer0A_IRQHandler()
{
    led_toggle(LED_GREEN);
    TIMER0->ICR |= (1U << 0);
    TIMER0->CTL &= ~(1U << 0);
}