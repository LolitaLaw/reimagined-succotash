#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// SPI 配置参数（ESP32-C3 引脚）
#define SPI_HOST_ID SPI2_HOST   // 使用 SPI2
#define SPI_MOSI_PIN GPIO_NUM_7 // MOSI 引脚
#define SPI_MISO_PIN GPIO_NUM_2 // MISO 引脚
#define SPI_SCLK_PIN GPIO_NUM_6 // 时钟引脚
#define SPI_CS_PIN GPIO_NUM_10  // 片选引脚

#define SPI_CLOCK_SPEED_HZ (1 * 1000 * 1000) // 1MHz 时钟频率
#define SPI_MAX_TRANSFER_SIZE 4092           // 最大传输字节数

// SPI 模式定义
typedef enum
{
    SPI_MODE_0 = 0, // CPOL=0, CPHA=0
    SPI_MODE_1 = 1, // CPOL=0, CPHA=1
    SPI_MODE_2 = 2, // CPOL=1, CPHA=0
    SPI_MODE_3 = 3  // CPOL=1, CPHA=1
} spi_mode_t;

// 函数声明
esp_err_t spi_init(void);
esp_err_t spi_transmit(const uint8_t *tx_data, size_t length);
esp_err_t spi_receive(uint8_t *rx_data, size_t length);
esp_err_t spi_transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t length);
esp_err_t spi_write_byte(uint8_t data);
uint8_t spi_read_byte(void);
void spi_cs_select(void);
void spi_cs_deselect(void);

#endif // __BSP_SPI_H__
