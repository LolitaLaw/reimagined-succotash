#include "bsp_spi_lock.h"

//  SPI 互斥量实现
static SemaphoreHandle_t s_spi_mutex = NULL;

// 初始化锁
void bsp_spi_lock_init(void)
{
    if (s_spi_mutex == NULL)
    {
        s_spi_mutex = xSemaphoreCreateMutex();
    }
}

// 获取锁
SemaphoreHandle_t bsp_spi_get_lock(void)
{
    return s_spi_mutex;
}

// 释放锁
void bsp_spi_free_lock(SemaphoreHandle_t lock)
{
    if (lock)
    {
        xSemaphoreGive(lock);
    }
}
