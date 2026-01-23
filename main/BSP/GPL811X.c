#include "GPL811X.h"

const char *GPL811x = "GPL811x";

uint8_t T_DirBuf[4] = {0x0};
uint8_t T_DataBuf[4] = {0x0};
// 寄存器——写
void F_GPL811Reg_WriteData(uint16_t RegAddr, uint8_t data)
{
    SPI2_WriteByte(RegAddr, data);
}
// 寄存器——读
uint8_t F_GPL811Reg_ReadData(uint16_t RegAddr)
{
    return SPI2_ReadByte(RegAddr);
}
// 寄存器——置位
void F_GPL811Reg_SetBits(uint16_t RegAddr, uint8_t bits)
{
    uint8_t ucVal;
    ucVal = SPI2_ReadByte(RegAddr);
    set(ucVal, bits);
    SPI2_WriteByte(RegAddr, ucVal);
}
// 寄存器——清位
void F_GPL811Reg_ClrBits(uint16_t RegAddr, uint8_t bits)
{
    uint8_t ucVal;
    ucVal = SPI2_ReadByte(RegAddr);
    clr(ucVal, bits);
    SPI2_WriteByte(RegAddr, ucVal);
}
// 寄存器——取反
void F_GPL811Reg_TogBits(uint16_t RegAddr, uint8_t bits)
{
    uint8_t ucVal;
    ucVal = SPI2_ReadByte(RegAddr);
    tog(ucVal, bits);
    SPI2_WriteByte(RegAddr, ucVal);
}
// 寄存器——位测试
uint8_t F_GPL811Reg_TstBits(uint16_t RegAddr, uint8_t bits)
{
    uint8_t ucVal;
    ucVal = SPI2_ReadByte(RegAddr);
    if (tst(ucVal, bits))
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

    io_conf.intr_type = GPIO_INTR_DISABLE; // 禁止中断
    io_conf.mode = GPIO_MODE_OUTPUT;       // 设置为输出模式
    io_conf.pin_bit_mask = (1ULL << SPI_SDI) | (1ULL << SPI_SCN) | (1ULL << SPI_CLK);
    io_conf.pull_down_en = 0; // 禁止下拉
    io_conf.pull_up_en = 0;   // 禁止上拉
    gpio_config(&io_conf);    // 开始配置管脚

    io_conf.intr_type = GPIO_INTR_DISABLE; // 禁止中断
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << SPI_SDO); // 管脚的位,io13
    io_conf.pull_down_en = 0;                 // 禁止下拉
    io_conf.pull_up_en = 0;                   // 禁止上拉
    gpio_config(&io_conf);                    // 开始配置管脚

    gpio_set_level(SPI_SDI, 0);
    gpio_set_level(SPI_CLK, 0);
    CS2_H();

    while (cnt--)
    {
        SPI2_WriteByte(0x300, 0x5a);
        if (SPI2_ReadByte(0x300) == 0x5a)
        {
            LOG_I(GPL811x, 1, "GPL811x Ready...");
            break;
        }
        LOG_E(GPL811x, 1, "GPL811x Not Ready...");
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

void lcd_init(void)
{
    // 配置 LCD: 1/8 duty, 1/4 bias, 6V
    soft_spi_write_byte(0x07, 0xC1);
    soft_spi_write_byte(0x19, 0x07);
    ESP_LOGI("GPL811x", "LCD 控制器初始化完成");

    // 按键和背光初始化
    F_GPL811Reg_WriteData(P_IO_Ctrl, 0);
    GPIOE_FLOAT_INPUT(0x01);
    GPIOE_OPENPD_INPUT(KEY_MASK + DC_PIN);

    SPI2_WriteByte(P_IO_PortD_Attrib, 0);
    GPIOD_OPENPD_INPUT(KEY_PD_MASK);
}
