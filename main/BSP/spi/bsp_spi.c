#include "bsp_spi.h"
/*
硬实现 SPI 驱动，基于 ESP-IDF SPI Master 驱动封装
*/
// SPI 设备句柄
static spi_device_handle_t spi_handle;

/**
 * @brief 初始化 SPI 总线和设备
 * @return ESP_OK 成功, 其他值表示失败
 */
esp_err_t spi_init(void)
{
    esp_err_t ret;

    // 配置 SPI 总线
    spi_bus_config_t bus_config = {
        .mosi_io_num = SPI_MOSI_PIN,              // MOSI 引脚
        .miso_io_num = SPI_MISO_PIN,              // MISO 引脚
        .sclk_io_num = SPI_SCLK_PIN,              // 时钟引脚
        .quadwp_io_num = -1,                      // WP 引脚（不使用）
        .quadhd_io_num = -1,                      // HD 引脚（不使用）
        .max_transfer_sz = SPI_MAX_TRANSFER_SIZE, // 最大传输大小
    };

    // 初始化 SPI 总线
    ret = spi_bus_initialize(SPI_HOST_ID, &bus_config, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK)
    {
        printf("SPI 总线初始化失败: %d\n", ret);
        return ret;
    }

    // 配置 SPI 设备
    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = SPI_CLOCK_SPEED_HZ, // 时钟频率
        .mode = 0,                            // SPI 模式 0 (CPOL=0, CPHA=0)
        .spics_io_num = SPI_CS_PIN,           // CS 引脚
        .queue_size = 7,                      // 事务队列大小
        .flags = 0,                           // 标志位
        .pre_cb = NULL,                       // 传输前回调
        .post_cb = NULL,                      // 传输后回调
    };

    // 添加设备到 SPI 总线
    ret = spi_bus_add_device(SPI_HOST_ID, &dev_config, &spi_handle);
    if (ret != ESP_OK)
    {
        printf("SPI 设备添加失败: %d\n", ret);
        return ret;
    }

    printf("SPI 初始化成功\n");
    return ESP_OK;
}

/**
 * @brief SPI 只发送数据
 * @param tx_data 要发送的数据缓冲区
 * @param length 数据长度
 * @return ESP_OK 成功
 */
esp_err_t spi_transmit(const uint8_t *tx_data, size_t length)
{
    if (length == 0 || tx_data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    spi_transaction_t trans = {
        .length = length * 8, // 总长度（位）
        .tx_buffer = tx_data, // 发送缓冲区
        .rx_buffer = NULL,    // 不接收
    };

    return spi_device_transmit(spi_handle, &trans);
}

/**
 * @brief SPI 只接收数据
 * @param rx_data 接收数据缓冲区
 * @param length 数据长度
 * @return ESP_OK 成功
 */
esp_err_t spi_receive(uint8_t *rx_data, size_t length)
{
    if (length == 0 || rx_data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    spi_transaction_t trans = {
        .length = length * 8, // 总长度（位）
        .tx_buffer = NULL,    // 不发送
        .rx_buffer = rx_data, // 接收缓冲区
        .flags = 0,
    };

    return spi_device_transmit(spi_handle, &trans);
}

/**
 * @brief SPI 全双工传输（同时发送和接收）
 * @param tx_data 要发送的数据缓冲区
 * @param rx_data 接收数据缓冲区
 * @param length 数据长度
 * @return ESP_OK 成功
 */
esp_err_t spi_transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t length)
{
    if (length == 0)
    {
        return ESP_ERR_INVALID_ARG;
    }

    spi_transaction_t trans = {
        .length = length * 8, // 总长度（位）
        .tx_buffer = tx_data, // 发送缓冲区
        .rx_buffer = rx_data, // 接收缓冲区
    };

    return spi_device_transmit(spi_handle, &trans);
}

/**
 * @brief 写入单个字节
 * @param data 要写入的字节
 * @return ESP_OK 成功
 */
esp_err_t spi_write_byte(uint8_t data)
{
    return spi_transmit(&data, 1);
}

/**
 * @brief 读取单个字节
 * @return 读取到的字节
 */
uint8_t spi_read_byte(void)
{
    uint8_t data = 0;
    spi_receive(&data, 1);
    return data;
}

/**
 * @brief 手动拉低 CS（选中设备）
 * @note 使用硬件 CS 时通常不需要手动控制
 */
void spi_cs_select(void)
{
    gpio_set_level(SPI_CS_PIN, 0);
}

/**
 * @brief 手动拉高 CS（取消选中设备）
 * @note 使用硬件 CS 时通常不需要手动控制
 */
void spi_cs_deselect(void)
{
    gpio_set_level(SPI_CS_PIN, 1);
}
