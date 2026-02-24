// *软实现 SPI 驱动，基于 GPIO 模拟 SPI 协议
#include "bsp_soft_spi.h"

static const char *TAG = "SOFT_SPI";         // 日志标签
static bool s_initialized = false;           // SPI 初始化标志
static SemaphoreHandle_t s_spi_mutex = NULL; // SPI 互斥量
// #define DELAY_US(us) ets_delay_us(us) // 微秒延时

// 局部
// 获取 SPI 总线访问权,带超时
static bool spi_lock(uint32_t timeout_ms)
{
    if (s_spi_mutex == NULL)
    {
        ESP_LOGE(TAG, "SPI 未初始化");
        return false;
    }

    if (xSemaphoreTakeRecursive(s_spi_mutex, pdMS_TO_TICKS(timeout_ms)) != pdTRUE)
    {
        ESP_LOGE(TAG, "获取 SPI 互斥量超时");
        return false;
    }

    return true;
}

// 释放 SPI 总线访问权
static void spi_unlock(void)
{
    if (s_spi_mutex != NULL)
        xSemaphoreGiveRecursive(s_spi_mutex);
}
// 全局接口
// 获取 SPI 互斥量
void bsp_spi_take_lock(void)
{
    if (s_spi_mutex)
        xSemaphoreTakeRecursive(s_spi_mutex, portMAX_DELAY);
}
// 释放 SPI 互斥量
void bsp_spi_give_lock(void)
{
    if (s_spi_mutex)
        xSemaphoreGiveRecursive(s_spi_mutex);
}
// 初始化软件 SPI
void soft_spi_init(void)
{
    if (s_initialized)
    {
        ESP_LOGW(TAG, "SPI 已初始化，跳过");
        return;
    }
    // 1. 创建递归互斥量
    s_spi_mutex = xSemaphoreCreateRecursiveMutex();
    if (s_spi_mutex == NULL)
    {
        ESP_LOGE(TAG, "创建互斥量失败");
        return;
    }

    // 2. 配置输出引脚（MOSI, SCLK, CS）
    gpio_config_t io_output = {
        .intr_type = GPIO_INTR_DISABLE, // 禁用中断
        .mode = GPIO_MODE_OUTPUT,       // 输出模式
        .pin_bit_mask = (1ULL << SOFT_SPI_MOSI_PIN) | (1ULL << SOFT_SPI_SCLK_PIN) | (1ULL << SOFT_SPI_CS_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&io_output);

    // 3. 配置输入引脚（MISO）
    gpio_config_t io_input = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << SOFT_SPI_MISO_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE, // 根据硬件决定是否启用上拉
    };
    gpio_config(&io_input);

    // 4. 设置初始电平状态
    MOSI_L();
    SCLK_L();
    CS_H();
    s_initialized = true;
    ESP_LOGI(TAG, "软件 SPI 初始化完成");
}

// 发送单个字节
void soft_spi_send_byte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        // 1. 在 CLK 低电平期间，检查最高位，决定 MOSI 电平
        if (data & 0x80)
            MOSI_H();
        else
            MOSI_L();
        SCLK_H(); // 2. CLK 拉高，从机在上升沿采样 MOSI
        ets_delay_us(6);
        data <<= 1; // 3. 数据左移
        SCLK_L();   // 4. CLK 拉低，完成一位的传输
        ets_delay_us(6);
    }
    MOSI_L(); // 传输完成后，MOSI 拉低（可选，保持总线安静）
}

// 接收单个字节
uint8_t soft_spi_recv_byte(void)
{
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        data <<= 1; // 1. 数据左移
        SCLK_H();   // 2. CLK 拉高，此时从机输出数据，主机采样
        ets_delay_us(6);
        if (MISO_READ())  // 3. 读取 MISO 引脚状态
            data |= 0x01; // 如果是高电平，最低位置 1
        SCLK_L();         // 4. CLK 拉低
        ets_delay_us(6);
    }
    return data;
}

// 设置 CS 引脚电平
void soft_spi_cs_set(int level)
{
    if (level)
    {
        CS_H();
    }
    else
    {
        CS_L();
    }
}

// 检查指针非空并加锁
static esp_err_t check_args_and_lock(const void *ptr)
{
    if (ptr == NULL)
        return ESP_ERR_INVALID_ARG;
    if (!spi_lock(1000))
        return ESP_ERR_TIMEOUT;
    return ESP_OK;
}

// 写入单个字节
void soft_spi_write_byte(uint16_t addr, uint8_t data)
{
    // 1. 获取总线访问权
    if (check_args_and_lock(&data) != ESP_OK)
        return;
    // 2. 片选拉低，开始传输
    CS_L();
    // 3. 发送 16 位地址（高字节在前）
    soft_spi_send_byte((uint8_t)(addr >> 8));
    soft_spi_send_byte((uint8_t)(addr & 0xFF));
    // 4. 发送 8 位数据
    soft_spi_send_byte(data);
    // 5. 片选拉高，结束传输
    CS_H();
    // 6. 释放总线访问权
    spi_unlock();
}

// 读取单个字节
void soft_spi_read_byte(uint16_t addr, uint8_t *data)
{
    if (check_args_and_lock(data) != ESP_OK)
        return;
    CS_L();
    // 发送地址，读操作需要将最高位置 1
    uint16_t read_addr = addr | SOFT_SPI_READ_FLAG;
    soft_spi_send_byte((uint8_t)(read_addr >> 8));
    soft_spi_send_byte((uint8_t)(read_addr & 0xFF));

    ets_delay_us(45); // 读操作需要额外延时，等待从机准备数据(这是 GPL811x 的特殊要求)
    // 读取数据
    *data = soft_spi_recv_byte();
    CS_H();
    spi_unlock();
}

// 写入多字节
void soft_spi_write_nbytes(uint16_t addr, const uint8_t *buf, uint16_t len)
{
    if (check_args_and_lock(buf) != ESP_OK)
        return;
    CS_L();
    // 发送地址
    soft_spi_send_byte((uint8_t)(addr >> 8));
    soft_spi_send_byte((uint8_t)(addr & 0xFF));
    // 连续发送数据，只需要一次 CS 切换，效率更高
    for (; len > 0; len--)
        soft_spi_send_byte(*buf++);
    CS_H();
    spi_unlock();
}
// 读取多字节
void soft_spi_read_nbytes(uint16_t addr, uint8_t *buf, uint16_t len)
{
    if (check_args_and_lock(buf) != ESP_OK)
        return;
    CS_L();
    // 发送地址（带读标志）
    uint16_t read_addr = addr | SOFT_SPI_READ_FLAG;
    soft_spi_send_byte((uint8_t)(read_addr >> 8));
    soft_spi_send_byte((uint8_t)(read_addr & 0xFF));
    ets_delay_us(45); // 读延时
    // 连续读取数据
    for (; len > 0; len--)
        *buf++ = soft_spi_recv_byte();
    CS_H();
    spi_unlock();
}

bool soft_spi_test_comm(void)
{
    const uint16_t test_addr = 0x0300; // GPL811x 可读写寄存器
    const uint8_t test_value = 0x5A;
    uint8_t read_value = 0;

    ESP_LOGI(TAG, "测试 SPI 通信...");

    soft_spi_write_byte(test_addr, test_value); /* 写测试 */
    soft_spi_read_byte(test_addr, &read_value); /* 读验证 */
    if (read_value != test_value)
    {
        ESP_LOGE(TAG, "数据不匹配: 写入 0x%02X, 读回 0x%02X", test_value, read_value);
        return false;
    }

    ESP_LOGI(TAG, "SPI 通信测试通过");
    return true;
}

// 测试软件 SPI 功能
void test_soft_spi()
{
    uint8_t value;
    uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    soft_spi_init();           // 1. 初始化
    if (!soft_spi_test_comm()) // 2. 测试通信
    {
        ESP_LOGI(TAG, "SPI 通信失败!");
        return;
    }
    soft_spi_write_byte(0x07, 0xC1);        // 3. 写寄存器
    soft_spi_read_byte(0x07, &value);       // 4. 读寄存器
    soft_spi_write_nbytes(0x3E00, data, 8); // 5. 批量写入 LCD RAM
}