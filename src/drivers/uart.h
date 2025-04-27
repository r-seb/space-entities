#pragma once

#include <stdint.h>

// Tiva C Launchpad Evaluation Kit user Manual
// 2.3.1 Virtual COM Port, pg. 12
// The TM4C123G eval kit comes with a Virtual COM PORT, UART Module 0 at GPIOA
#define U0RX (1U << 0)
#define U0TX (1U << 1)

// UART Config
#define UART_9600 9600U
#define UART_57600 57600U
#define UART_115200 115200U
#define UART_921600 921600U

void uart_init(uint32_t baud_rate);
int uart_send(const char* format, ...);
void uart_fill_fifo(char** c);
