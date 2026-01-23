/**
 * @file bsp_soft_spi.h
 * @brief 软件模拟 SPI 驱动（针对 GPL811x LCD 控制器）
 *
 * 为什么使用软件 SPI：
 * 1. 引脚灵活：可以使用任意 GPIO，不受硬件 SPI 引脚限制
 * 2. 时序可控：某些芯片有特殊时序要求，软件 SPI 可以精确控制
 * 3. 多设备：当硬件 SPI 资源不足时，软件 SPI 可以扩展
 * 4. 调试方便：可以逐位观察信号，便于调试
 *
 * 缺点：
 * 1. 速度较慢：相比硬件 SPI，软件模拟速度慢 10-100 倍
 * 2. CPU 占用高：需要 CPU 全程参与，无法使用 DMA
 * 3. 时钟精度：依赖软件延时，精度不如硬件
 */

#ifndef __BSP_SOFT_SPI_H__
#define __BSP_SOFT_SPI_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/*============================================================================
 *                              配置区域
 * 说明：根据硬件连接修改这里的引脚定义
 *===========================================================================*/

/**
 * SPI 引脚定义（软件 SPI 可以使用任意 GPIO）
 *
 * 命名说明：
 * - MOSI (Master Out Slave In)：主机输出，从机输入
 * - MISO (Master In Slave Out)：主机输入，从机输出
 * - SCLK (Serial Clock)：时钟信号，由主机产生
 * - CS (Chip Select)：片选信号，低电平有效
 */
#define SOFT_SPI_MOSI_PIN GPIO_NUM_7 // 主机输出 → GPL811x SDI
#define SOFT_SPI_MISO_PIN GPIO_NUM_1 // 主机输入 ← GPL811x SDO
#define SOFT_SPI_SCLK_PIN GPIO_NUM_6 // 时钟信号
#define SOFT_SPI_CS_PIN GPIO_NUM_18  // 片选信号

/**
 * SPI 时序参数（单位：微秒）
 *
 * 为什么需要延时：
 * 1. 建立时间：数据线在时钟边沿前需要稳定
 * 2. 保持时间：数据线在时钟边沿后需要保持
 * 3. 片选延时：某些芯片需要 CS 拉低后等待才能通信
 *
 * GPL811x 要求：
 * - 时钟周期 > 12us（6us 高 + 6us 低）
 * - CS 拉低后等待 > 80us
 * - CS 拉高前等待 > 30us
 */
#define SOFT_SPI_CLK_DELAY_US 6   // 时钟半周期延时（约 83kHz）
#define SOFT_SPI_CS_SETUP_US 80   // CS 拉低后的建立时间
#define SOFT_SPI_CS_HOLD_US 30    // CS 拉高前的保持时间
#define SOFT_SPI_CS_IDLE_US 45    // CS 拉高后的空闲时间
#define SOFT_SPI_READ_DELAY_US 45 // 读操作的额外延时

/**
 * GPL811x 寄存器地址特点：
 * - 读操作：地址最高位置 1（| 0x8000）
 * - 写操作：地址最高位为 0
 */
#define SOFT_SPI_READ_FLAG 0x8000

// SPI 传输模式
typedef enum
{                        //  极性    相位
    SOFT_SPI_MODE_0 = 0, // CPOL=0, CPHA=0  空闲低电平，上升沿采样
    SOFT_SPI_MODE_1 = 1, // CPOL=0, CPHA=1  空闲低电平，下降沿采样
    SOFT_SPI_MODE_2 = 2, // CPOL=1, CPHA=0  空闲高电平，下降沿采样
    SOFT_SPI_MODE_3 = 3, // CPOL=1, CPHA=1  空闲高电平，上升沿采样
} soft_spi_mode_t;

// SPI 配置结构体
typedef struct
{
    gpio_num_t mosi_pin;  // MOSI 引脚
    gpio_num_t miso_pin;  // MISO 引脚
    gpio_num_t sclk_pin;  // SCLK 引脚
    gpio_num_t cs_pin;    // CS 引脚
    soft_spi_mode_t mode; // SPI 模式
    uint8_t clk_delay_us; // 时钟延时
} soft_spi_config_t;

// 初始化SPI
esp_err_t soft_spi_init(void);

// 释放软件 SPI 资源
esp_err_t soft_spi_deinit(void);

/**
 * @brief 写入单字节到指定地址
 *
 * @param addr 16位地址
 * @param data 要写入的数据
 * @return ESP_OK 成功
 *
 * 通信协议（GPL811x）：
 * [CS拉低] → [地址高8位] → [地址低8位] → [数据] → [CS拉高]
 */
esp_err_t soft_spi_write_byte(uint16_t addr, uint8_t data);

/**
 * @brief 从指定地址读取单字节
 *
 * @param addr 16位地址
 * @param data 读取数据的指针
 * @return ESP_OK 成功
 *
 * 通信协议（GPL811x）：
 * [CS拉低] → [地址|0x8000高8位] → [地址低8位] → [延时] → [读数据] → [CS拉高]
 */
esp_err_t soft_spi_read_byte(uint16_t addr, uint8_t *data);

/**
 * @brief 写入多字节到指定地址
 *
 * @param addr 起始地址
 * @param buf 数据缓冲区
 * @param len 数据长度
 * @return ESP_OK 成功
 */
esp_err_t soft_spi_write_nbytes(uint16_t addr, const uint8_t *buf, uint16_t len);

// 从指定地址读取多字节
esp_err_t soft_spi_read_nbytes(uint16_t addr, uint8_t *buf, uint16_t len);

/**
 * @brief 检查设备通信是否正常
 *
 * @return true 通信正常，false 通信失败
 *
 * 测试方法：
 * 向可读写寄存器写入特定值，再读回验证
 */
bool soft_spi_test_comm(void);

/**
 * @brief 发送单个字节（无 CS 控制）
 * @param data 要发送的字节
 *
 * 时序图（Mode 0）：
 *       ___     ___     ___     ___
 * CLK  |   |___|   |___|   |___|   |___
 *      ^       ^       ^       ^
 *      |       |       |       |
 * MOSI D7      D6      D5      D4 ...
 *      ↑采样点
 *
 * 说明：
 * 1. MSB First：最高位先发
 * 2. 在 CLK 上升沿，从机采样 MOSI
 * 3. 每位数据在下降沿更新
 */
void soft_spi_send_byte(uint8_t data);

/**
 * @brief 接收单个字节（无 CS 控制）
 * @return 接收到的字节
 *
 * 时序图（Mode 0）：
 *       ___     ___     ___     ___
 * CLK  |   |___|   |___|   |___|   |___
 *          ^       ^       ^       ^
 *          |       |       |       |
 * MISO    D7      D6      D5      D4 ...
 *          ↑采样点
 */
uint8_t soft_spi_recv_byte(void);

/**
 * @brief 手动控制 CS 引脚
 * @param level 0=低电平（选中），1=高电平（释放）
 */
void soft_spi_cs_set(int level);

#endif /* __BSP_SOFT_SPI_H__ */
