#ifndef __BSP_SPI2_H__
#define __BSP_SPI2_H__

#include "rom/ets_sys.h"
#include "driver/gpio.h"
#include <soc/cpu.h>

#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "bsp/bsp_config.h"

//-----------------------------------------------------------------
// for GPL811
//-----------------------------------------------------------------
// SDI SDO是对MCU来说
#define SPI_SDI GPIO_NUM_7
#define SPI_SDO GPIO_NUM_1
#define SPI_SCN GPIO_NUM_18
#define SPI_CLK GPIO_NUM_6

#define SPI_SDI_Mask ((uint64_t)(((uint64_t)1) << SPI_SDI))
#define SPI_SDO_Mask ((uint64_t)(((uint64_t)1) << SPI_SDO))
#define SPI_SCN_Mask ((uint64_t)(((uint64_t)1) << SPI_SCN))
#define SPI_CLK_Mask ((uint64_t)(((uint64_t)1) << SPI_CLK))

#define MOSI_Low()                                 \
    gpio_set_direction(SPI_SDI, GPIO_MODE_OUTPUT); \
    gpio_set_level(SPI_SDI, 0)
#define MOSI_High()                                \
    gpio_set_direction(SPI_SDI, GPIO_MODE_OUTPUT); \
    gpio_set_level(SPI_SDI, 1)
#define SCK_Low() gpio_set_level(SPI_CLK, 0)
#define SCK_High() gpio_set_level(SPI_CLK, 1)
#define READ_MISO() gpio_get_level(SPI_SDO)

#define GPL811_delay() ets_delay_us(45)
#define CS2_L()                 \
    gpio_set_level(SPI_SCN, 0); \
    ets_delay_us(80)
#define CS2_H()                 \
    ets_delay_us(30);           \
    gpio_set_level(SPI_SCN, 1); \
    ets_delay_us(45)

extern SemaphoreHandle_t spi_mutex;

void init_spi_mutex(void);

void SPI_Send8bit(uint8_t data);
uint8_t SPI2_ReadByte(uint16_t Addr);
void SPI2_WriteByte(uint16_t Addr, uint8_t data);
void SPI2_Read_N_Byte(uint16_t Addr, uint8_t *buf, uint16_t cnt);
void SPI2_Write_N_Byte(uint16_t Addr, uint8_t *buf, uint16_t cnt);

#endif // BSP_SPI2