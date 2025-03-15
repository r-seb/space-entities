#include "TM4C123GH6PM.h" /* the TM4C MCU Peripheral Access Layer (TI) */
#include <stdint.h>

#define SYS_CLOCK_HZ 16000000U

/* on-board LEDs */
#define LED_RED (1U << 1)
#define LED_BLUE (1U << 2)
#define LED_GREEN (1U << 3)

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

int main(void)
{
    SYSCTL->RCGCGPIO |= (1U << 5);  /* enable Run mode for GPIOF */
    SYSCTL->GPIOHBCTL |= (1U << 5); /* enable AHB for GPIOF */
    GPIOF_AHB->DIR |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIOF_AHB->DEN |= (LED_RED | LED_BLUE | LED_GREEN);

    while (1) {
        int32_t count = 0;
        GPIOF_AHB->DATA_Bits[LED_GREEN] = LED_GREEN; // LED ON
        while (count < 1000000) {
            count++;
        }

        count = 0;
        GPIOF_AHB->DATA_Bits[LED_GREEN] = ~LED_GREEN; // LED OFF
        while (count < 1000000) {
            count++;
        }
    }
}