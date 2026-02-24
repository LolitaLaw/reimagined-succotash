#include "GPL811X.h"

const char *GPL811x = "GPL811x";

uint8_t T_DirBuf[4] = {0x0};
uint8_t T_DataBuf[4] = {0x0};
// 寄存器——写
void F_GPL811Reg_WriteData(uint16_t RegAddr, uint8_t data)
{
    soft_spi_write_byte(RegAddr, data);
}
// 寄存器——读
uint8_t F_GPL811Reg_ReadData(uint16_t RegAddr)
{
    uint8_t data;
    soft_spi_read_byte(RegAddr, &data);
    if (data)
    {
        ESP_LOGE("SPI", "读取寄存器失败");
        return 0;
    }
    else
        return data;
}
// 寄存器——置位
void F_GPL811Reg_SetBits(uint16_t RegAddr, uint8_t bits)
{
    uint8_t data;
    soft_spi_read_byte(RegAddr, &data);
    set(data, bits);
    soft_spi_write_byte(RegAddr, data);
}
// 寄存器——清位
void F_GPL811Reg_ClrBits(uint16_t RegAddr, uint8_t bits)
{
    uint8_t data;
    soft_spi_read_byte(RegAddr, &data);
    clr(data, bits);
    soft_spi_write_byte(RegAddr, data);
}
// 寄存器——取反
void F_GPL811Reg_TogBits(uint16_t RegAddr, uint8_t bits)
{
    uint8_t data;
    soft_spi_read_byte(RegAddr, &data);
    tog(data, bits);
    soft_spi_write_byte(RegAddr, data);
}
// 寄存器——位测试
uint8_t F_GPL811Reg_TstBits(uint16_t RegAddr, uint8_t bits)
{
    uint8_t data;
    soft_spi_read_byte(RegAddr, &data);
    if (tst(data, bits))
    {
        return 1;
    }
    return 0;
}
// 等待GPL811X准备就绪
void F_WaitGPL811Ready(void)
{
    gpio_config_t io_conf;
    uint8_t cnt = 10;
    bsp_spi_take_lock(); // 获取 SPI 互斥量

    /* 配置 GPIO */
    io_conf.intr_type = GPIO_INTR_DISABLE; // 禁止中断
    io_conf.mode = GPIO_MODE_OUTPUT;       // 设置为输出模式
    io_conf.pin_bit_mask = (1ULL << SOFT_SPI_MOSI_PIN) | (1ULL << SOFT_SPI_CS_PIN) | (1ULL << SOFT_SPI_SCLK_PIN);
    io_conf.pull_down_en = 0; // 禁止下拉
    io_conf.pull_up_en = 0;   // 禁止上拉
    gpio_config(&io_conf);    // 开始配置管脚

    io_conf.intr_type = GPIO_INTR_DISABLE; // 禁止中断
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << SOFT_SPI_MISO_PIN); // 管脚的位,io13
    io_conf.pull_down_en = 0;                           // 禁止下拉
    io_conf.pull_up_en = 0;                             // 禁止上拉
    gpio_config(&io_conf);                              // 开始配置管脚

    gpio_set_level(SOFT_SPI_MOSI_PIN, 0);
    gpio_set_level(SOFT_SPI_SCLK_PIN, 0);
    CS_H();

    while (cnt--)
    {
        soft_spi_write_byte(0x300, 0x5a);
        uint8_t data;
        soft_spi_read_byte(0x300, &data);
         if (data== 0x5a)
        {
            LOG_I(GPL811x, 1, "GPL811x Ready...");
            break;
        }
        LOG_E(GPL811x, 1, "GPL811x Not Ready...");
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    bsp_spi_give_lock(); // 释放 SPI 互斥量
}