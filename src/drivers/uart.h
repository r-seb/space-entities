#pragma once

// Tiva C Launchpad Evaluation Kit user Manual
// 2.3.1 Virtual COM Port, pg. 12 
// The TM4C123G eval kit comes with a Virtual COM PORT, UART Module 0 at GPIOA
#define U0RX (1U << 0)
#define U0TX (1U << 1)

// UART Config
#define UART_BAUD_RATE 115200U

void uart_init(void);
void putchar_(char c); // Needed for eyalroz/printf