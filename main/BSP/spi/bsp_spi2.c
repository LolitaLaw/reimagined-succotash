#include "bsp_spi2.h"

static const char *TAG = "RF_SPI";
SemaphoreHandle_t spi_mutex = NULL; // 互斥量，

void init_spi_mutex(void)
{
    spi_mutex = xSemaphoreCreateMutex();
    if (spi_mutex == NULL)
    {
        ESP_LOGE(TAG, "spi_mutex creat error!");
    }
}

void SPI_Send8bit(uint8_t data)
{
    uint8_t bitcnt;
    for (bitcnt = 0; bitcnt < 8; bitcnt++)
    {
        // 准备数据
        if (data & 0x80)
        {
            MOSI_High();
        }
        else
        {
            MOSI_Low();
        }
        SCK_High();
        ets_delay_us(6);
        data <<= 1;
        SCK_Low();
        ets_delay_us(6);
    }
    MOSI_Low();
}

uint8_t SPI_Read8bit(void)
{
    uint8_t bitcnt;
    uint8_t data = 0;
    for (bitcnt = 0; bitcnt < 8; bitcnt++)
    {
        data <<= 1;
        SCK_High();
        ets_delay_us(6);
        if (READ_MISO())
        {
            data |= 0x01; // NRZ MSB
        }
        SCK_Low();
        ets_delay_us(6);
    }
    return data;
}
// 读一个字节
uint8_t SPI2_ReadByte(uint16_t Addr)
{
    uint8_t data;
    if (pdTRUE != xSemaphoreTake(spi_mutex, pdMS_TO_TICKS(1000)))
    {
        LOG_E(TAG, 1, "xSemaphoreTake return error!");
        return 0;
    }

    CS2_L();
    Addr |= 0x8000;
    SPI_Send8bit((uint8_t)(Addr >> 8));
    SPI_Send8bit((uint8_t)(Addr & 0x00ff));
    GPL811_delay();
    data = SPI_Read8bit();
    CS2_H();
    xSemaphoreGive(spi_mutex);

    return data;
}
// 写一个字节
void SPI2_WriteByte(uint16_t Addr, uint8_t data)
{
    if (pdTRUE != xSemaphoreTake(spi_mutex, pdMS_TO_TICKS(1000)))
    {
        LOG_E(TAG, 1, "xSemaphoreTake return error!");
        return;
    }

    CS2_L();

    SPI_Send8bit((uint8_t)(Addr >> 8));
    SPI_Send8bit((uint8_t)(Addr & 0x00ff));
    SPI_Send8bit(data);
    CS2_H();
    xSemaphoreGive(spi_mutex);
}

void SPI2_Read_N_Byte(uint16_t Addr, uint8_t *buf, uint16_t cnt)
{
    if (pdTRUE != xSemaphoreTake(spi_mutex, pdMS_TO_TICKS(1000)))
    {
        LOG_E(TAG, 1, "xSemaphoreTake return error!");
        return;
    }

    CS2_L();
    Addr |= 0x8000;
    SPI_Send8bit((uint8_t)(Addr >> 8));
    SPI_Send8bit((uint8_t)(Addr & 0x00ff));
    GPL811_delay();
    for (; cnt > 0; cnt--)
    {
        *buf++ = SPI_Read8bit();
    }
    CS2_H();
    xSemaphoreGive(spi_mutex);
}

void SPI2_Write_N_Byte(uint16_t Addr, uint8_t *buf, uint16_t cnt)
{
    if (pdTRUE != xSemaphoreTake(spi_mutex, pdMS_TO_TICKS(1000)))
    {
        LOG_E(TAG, 1, "xSemaphoreTake return error!");
        return;
    }

    CS2_L();
    SPI_Send8bit((uint8_t)(Addr >> 8));
    SPI_Send8bit((uint8_t)(Addr & 0x00ff));
    for (; cnt > 0; cnt--)
    {
        SPI_Send8bit(*buf++);
    }
    CS2_H();
    xSemaphoreGive(spi_mutex);
}
