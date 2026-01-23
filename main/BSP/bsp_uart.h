#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"

#define UART_NUM UART_NUM_0   // 串口端口号
#define BUF_SIZE (1024)       // 缓冲区大小
#define TXD_PIN (GPIO_NUM_21) // 根据您的硬件修改
#define RXD_PIN (GPIO_NUM_20) // 根据您的硬件修改

void uart_init(void);
void uart_send_data(const char *data);
void uart_receive_task(void *arg);

#endif // BSP_UART