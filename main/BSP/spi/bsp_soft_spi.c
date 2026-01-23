// *软实现 SPI 驱动，基于 GPIO 模拟 SPI 协议
#include "bsp_soft_spi.h"
#include "rom/ets_sys.h"
#include "esp_log.h"

static const char *TAG = "SOFT_SPI";

/**
 * SPI 互斥量
 * 为什么使用互斥量而不是二值信号量：
 * 1. 互斥量有优先级继承机制，可以防止优先级反转
 * 2. 互斥量只能由获取它的任务释放，更安全
 * 3. 适合保护共享资源（如 SPI 总线）
 */
static SemaphoreHandle_t s_spi_mutex = NULL;
static bool s_initialized = false; // SPI 初始化标志

// MOSI 引脚操作
#define MOSI_H() gpio_set_level(SOFT_SPI_MOSI_PIN, 1)
#define MOSI_L() gpio_set_level(SOFT_SPI_MOSI_PIN, 0)

// SCLK 引脚操作
#define SCLK_H() gpio_set_level(SOFT_SPI_SCLK_PIN, 1)
#define SCLK_L() gpio_set_level(SOFT_SPI_SCLK_PIN, 0)

// MISO 引脚读取
#define MISO_READ() gpio_get_level(SOFT_SPI_MISO_PIN)

// CS 引脚操作(低电平有效)
#define CS_L()                          \
    gpio_set_level(SOFT_SPI_CS_PIN, 0); \
    ets_delay_us(80)

#define CS_H()                          \
    ets_delay_us(30);                   \
    gpio_set_level(SOFT_SPI_CS_PIN, 1); \
    ets_delay_us(45)

// #define DELAY_US(us) ets_delay_us(us) // 微秒延时

/*
 * @brief 获取 SPI 总线访问权
 * @param timeout_ms 超时时间（毫秒）
 * @return true 获取成功，false 超时
 * @note 防止死锁：如果某个任务出错没有释放互斥量，其他任务不会永远等待
 */
static bool spi_lock(uint32_t timeout_ms)
{
    if (s_spi_mutex == NULL)
    {
        ESP_LOGE(TAG, "SPI 未初始化");
        return false;
    }

    if (xSemaphoreTake(s_spi_mutex, pdMS_TO_TICKS(timeout_ms)) != pdTRUE)
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
        xSemaphoreGive(s_spi_mutex);
}
// 初始化软件 SPI
esp_err_t soft_spi_init(void)
{
    if (s_initialized)
    {
        ESP_LOGW(TAG, "SPI 已初始化，跳过");
        return ESP_OK;
    }
    // 1. 创建互斥量
    s_spi_mutex = xSemaphoreCreateMutex();
    if (s_spi_mutex == NULL)
    {
        ESP_LOGE(TAG, "创建互斥量失败");
        return ESP_ERR_NO_MEM;
    }

    // 2. 配置输出引脚（MOSI, SCLK, CS）
    gpio_config_t io_output = {
        .intr_type = GPIO_INTR_DISABLE, // 禁用中断
        .mode = GPIO_MODE_OUTPUT,       // 输出模式
        .pin_bit_mask = (1ULL << SOFT_SPI_MOSI_PIN) |
                        (1ULL << SOFT_SPI_SCLK_PIN) |
                        (1ULL << SOFT_SPI_CS_PIN),
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

    /* 4. 设置初始电平状态
     *
     * Mode 0 的空闲状态：
     * - MOSI: 低电平（或高阻）
     * - SCLK: 低电平（CPOL=0）
     * - CS:   高电平（未选中）
     */
    MOSI_L();
    SCLK_L();
    CS_H();

    s_initialized = true;
    ESP_LOGI(TAG, "软件 SPI 初始化完成");

    return ESP_OK;
}
// 释放软件 SPI 资源
esp_err_t soft_spi_deinit(void)
{
    if (!s_initialized)
        return ESP_OK;
    if (s_spi_mutex != NULL)
    {
        vSemaphoreDelete(s_spi_mutex);
        s_spi_mutex = NULL;
    }
    s_initialized = false;
    ESP_LOGI(TAG, "软件 SPI 已释放");
    return ESP_OK;
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
        // 2. CLK 拉高，从机在上升沿采样 MOSI
        SCLK_H();
        ets_delay_us(6);
        // 3. 数据左移，准备下一位
        data <<= 1;
        // 4. CLK 拉低，完成一位的传输
        SCLK_L();
        ets_delay_us(6);
    }
    // 传输完成后，MOSI 拉低（可选，保持总线安静）
    MOSI_L();
}

// 接收单个字节
uint8_t soft_spi_recv_byte(void)
{
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        // 1. 数据左移，为新位腾出空间
        data <<= 1;
        // 2. CLK 拉高，此时从机输出数据，主机采样
        SCLK_H();
        ets_delay_us(6);
        // 3. 读取 MISO 引脚状态
        if (MISO_READ())
            data |= 0x01; // 如果是高电平，最低位置 1
        // 4. CLK 拉低
        SCLK_L();
        ets_delay_us(6);
    }
    return data;
}

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

// 写入单个字节
esp_err_t soft_spi_write_byte(uint16_t addr, uint8_t data)
{
    // 1. 获取总线访问权
    if (!spi_lock(1000))
        return ESP_ERR_TIMEOUT;
    // 2. 片选拉低，开始传输
    CS_L();
    // 3. 发送 16 位地址（高字节在前）
    //    写操作：地址最高位为 0，直接发送
    soft_spi_send_byte((uint8_t)(addr >> 8));
    soft_spi_send_byte((uint8_t)(addr & 0xFF));
    // 4. 发送 8 位数据
    soft_spi_send_byte(data);
    // 5. 片选拉高，结束传输
    CS_H();
    // 6. 释放总线访问权
    spi_unlock();
    return ESP_OK;
}
// 读取单个字节
esp_err_t soft_spi_read_byte(uint16_t addr, uint8_t *data)
{
    if (data == NULL)
        return ESP_ERR_INVALID_ARG;
    if (!spi_lock(1000))
        return ESP_ERR_TIMEOUT;
    CS_L();
    // 发送地址，读操作需要将最高位置 1
    uint16_t read_addr = addr | SOFT_SPI_READ_FLAG;
    soft_spi_send_byte((uint8_t)(read_addr >> 8));
    soft_spi_send_byte((uint8_t)(read_addr & 0xFF));

    // 读操作需要额外延时，等待从机准备数据(这是 GPL811x 的特殊要求)
    ets_delay_us(45);
    // 读取数据
    *data = soft_spi_recv_byte();
    CS_H();
    spi_unlock();
    return ESP_OK;
}
// 写入多字节
esp_err_t soft_spi_write_nbytes(uint16_t addr, const uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0)
        return ESP_ERR_INVALID_ARG;
    if (!spi_lock(1000))
        return ESP_ERR_TIMEOUT;
    CS_L();

    // 发送地址
    soft_spi_send_byte((uint8_t)(addr >> 8));
    soft_spi_send_byte((uint8_t)(addr & 0xFF));

    // 连续发送数据，只需要一次 CS 切换，效率更高
    for (uint16_t i = 0; i < len; i++)
        soft_spi_send_byte(buf[i]);
    CS_H();
    spi_unlock();
    return ESP_OK;
}
// 读取多字节
esp_err_t soft_spi_read_nbytes(uint16_t addr, uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0)
        return ESP_ERR_INVALID_ARG;
    if (!spi_lock(1000))
        return ESP_ERR_TIMEOUT;
    CS_L();

    // 发送地址（带读标志）
    uint16_t read_addr = addr | SOFT_SPI_READ_FLAG;
    soft_spi_send_byte((uint8_t)(read_addr >> 8));
    soft_spi_send_byte((uint8_t)(read_addr & 0xFF));

    // 读延时
    ets_delay_us(45);

    // 连续读取数据
    for (uint16_t i = 0; i < len; i++)
        buf[i] = soft_spi_recv_byte();

    CS_H();
    spi_unlock();

    return ESP_OK;
}

bool soft_spi_test_comm(void)
{
    const uint16_t test_addr = 0x0300; // GPL811x 可读写寄存器
    const uint8_t test_value = 0x5A;
    uint8_t read_value = 0;

    ESP_LOGI(TAG, "测试 SPI 通信...");

    /* 写入测试值 */
    if (soft_spi_write_byte(test_addr, test_value) != ESP_OK)
    {
        ESP_LOGE(TAG, "写入失败");
        return false;
    }

    /* 读回验证 */
    if (soft_spi_read_byte(test_addr, &read_value) != ESP_OK)
    {
        ESP_LOGE(TAG, "读取失败");
        return false;
    }

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
    // 1. 初始化
    soft_spi_init();
    // 2. 测试通信
    if (!soft_spi_test_comm())
    {
        printf("SPI 通信失败!\n");
        return;
    }
    // 3. 写寄存器
    soft_spi_write_byte(0x07, 0xC1);
    // 4. 读寄存器
    uint8_t value;
    soft_spi_read_byte(0x07, &value);
    // 5. 批量写入 LCD RAM
    uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    soft_spi_write_nbytes(0x3E00, data, 8);
}