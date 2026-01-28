#ifndef __BSP_SPI_LOCK_H__
#define __BSP_SPI_LOCK_H__

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// 初始化锁
void bsp_spi_lock_init(void);

// 获取锁
SemaphoreHandle_t bsp_spi_get_lock(void);

// 释放锁
void bsp_spi_free_lock(SemaphoreHandle_t lock);

#endif // BSP_SPI_LOCK