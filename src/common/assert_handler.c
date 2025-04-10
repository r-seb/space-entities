#include "assert_handler.h"
#include "TM4C123GH6PM.h"

_Noreturn void assert_failed(char const* const module, int const id)
{
    (void)module; // unused parameter
    (void)id;     // unused parameter
#ifndef NDEBUG
    // light up all LEDs
    GPIOF_AHB->DATA_Bits[(1U << 3) | (1U << 2) | (1U << 1)] = 0xFFU;
    // for debugging, hang on in an endless loop...
    for (;;) {}
#endif
    NVIC_SystemReset();
}
