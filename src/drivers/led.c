#include "led.h"
#include "TM4C123GH6PM.h" /* the TM4C MCU Peripheral Access Layer (TI) */
#include <stdint.h>

void led_init()
{
    SYSCTL->RCGCGPIO |= (1U << 5);  /* enable Run mode for GPIOF */
    SYSCTL->GPIOHBCTL |= (1U << 5); /* enable AHB for GPIOF */
    GPIOF_AHB->DIR |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIOF_AHB->DEN |= (LED_RED | LED_BLUE | LED_GREEN);
}