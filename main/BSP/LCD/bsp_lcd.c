#include "bsp_lcd.h"
static const char *TAG = "LCD";

// 背光引脚
#define BACKLED_GPIO GPIO_NUM_3

// LCD RAM
#define LCD_RAM_LENGTH 64
#define LCD_RAM_ADDR 0x3E00

// T_Zimu[0x41]处保存的数即为 字符 'A' 点亮LCD时需要的值
uint16_t T_Zimu[0x5B] =
    {
        // 0x00 - 0x2f 暂不建模
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00C0, 0x0000, 0x0000,

        // 0x30 - 0x39 : 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
        0x003f, // '0'
        0x0006, // '1'
        0x00db, // '2'
        0x00cf, // '3'
        0x00e6, // '4'
        0x00ed, // '5'
        0x00fd, // '6'
        0x0007, // '7'
        0x00ff, // '8'
        0x00ef, // '9'

        // 0x3a - 0x40 暂不建模
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

        // 0x41 - 0x5a
        0x00f7, // 'A'
        0x118f, // 'B'
        0x0039, // 'C'
        0x110f, // 'D'
        0x00f9, // 'E'
        0x00f1, // 'F'
        0x00bd, // 'G'
        0x00f6, // 'H'
        0x1109, // 'I'
        0x000e, // 'J'
        0x0630, //  0x1700,      // 'K' C30
        0x0038, // 'L'
        0x0137, //  0x0a36,      // 'M'
        0x0037, //  0x0c36,  // 'N'
        0x003f, // 'O'
        0x00f3, // 'P'
        0x00e7, // 'Q'
        0x04f3, // 'R'
        0x00ed, // 'S'
        0x1101, // 'T'
        0x003e, // 'U'
        0x2230, // 'V'
        0x103e, // 0x2436,      // 'W'
        0x2e00, // 'X'
        0x1a00, // 'Y'
        0x2209  // 'Z'
};

const uint8_t T_Shuzi[Shuzi_Max] =
    {
        0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, // 0 to 9
        0x77, 0x7c, 0x39, 0x5E, 0x79, 0x71, 0x76, 0x06, 0x38, 0x37, // A b C d E F H I L N
        0x3f, 0x73, 0x3e, 0x63, 0x5c,                               // O,P,U 上o 下o
        0x40, 0x00                                                  //-  NULL
};

static uint8_t lcd_ram[LCD_RAM_LENGTH + 1]; // LCD显存
uint8_t R_Bcd[6];                           // BCD结果存储: [个位, 十位, 百位, 千位, 万位, 符号]

void lcd_init(void)
{
    F_WaitGPL811Ready();

    // 配置 LCD: 1/8 duty, 1/4 bias, 6V
    soft_spi_write_byte(0x07, 0xC1);
    soft_spi_write_byte(0x19, 0x07);

    backlight_init(); // 背光初始化
    lcd_clear();      // 清屏

    ESP_LOGI("LCD", "LCD 控制器初始化完成");
}

/*============================================================================
 *                              背光控制
 *===========================================================================*/

void backlight_init(void)
{
    ledc_timer_config_t timer = {
        .duty_resolution = LEDC_TIMER_3_BIT,
        .freq_hz = 40000,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = BACKLED_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_1,
    };
    ledc_channel_config(&channel);
    backlight_set(0); // 关闭背光
}

void backlight_set(uint8_t level)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, level);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

/*============================================================================
 *                              LCD 控制
 *===========================================================================*/
// 更新 LCD 显存到屏幕
void lcd_update(void)
{
    uint16_t addr = LCD_RAM_ADDR;             // RAM 起始地址
    static uint8_t lcd_cache[LCD_RAM_LENGTH]; // 本地缓存

    bsp_spi_take_lock(); // 获取 SPI 互斥量

    for (int i = 0; i < LCD_RAM_LENGTH; i += 8)
    {
        if (memcmp(&lcd_cache[i], &lcd_ram[i], 8) != 0)
        {
            soft_spi_write_nbytes(addr + i, &lcd_ram[i], 8); // 写入硬件 LCD RAM
            memcpy(&lcd_cache[i], &lcd_ram[i], 8);           // 更新本地缓存
        }
    }
    bsp_spi_give_lock(); // 释放 SPI 互斥量
}

void lcd_full_on(void)
{
    memset(lcd_ram, 0xFF, LCD_RAM_LENGTH);
    lcd_update();
    ESP_LOGI(TAG, "LCD 全显示");
}

void lcd_clear(void)
{
    memset(lcd_ram, 0x00, LCD_RAM_LENGTH);
    lcd_update();
    ESP_LOGI(TAG, "LCD 清屏");
}

void lcd_display(uint16_t addr, bool state)
{
    Word temp;
    uint16_t addr_offset; // 地址偏移
    uint8_t bit_offset;   // 位偏移
    temp.Val = addr;
    addr_offset = temp.Byte.Byte1;
    bit_offset = temp.Byte.Byte0;

    if (state)
        set(lcd_ram[addr_offset], bit_offset);
    else
        clr(lcd_ram[addr_offset], bit_offset);
}

// 7段数码管显示数字
void lcd_displayShuzi(const uint16_t *addr, uint8_t num)
{
    uint8_t i;
    uint8_t temp;
    temp = T_Shuzi[num]; // 在T_Shuzi数组里查找对应的段码

    for (i = 0; i < 7; i++)
    {
        lcd_display(addr[i], temp & 0x01); // 完整写入
        temp >>= 1;
    }
}

// BCD 无效显示
void display_bcd_invalid(void)
{
    R_Bcd[5] = Shuzi_NEG;
    R_Bcd[4] = Shuzi_NEG;
    R_Bcd[3] = Shuzi_NEG;
    R_Bcd[2] = Shuzi_NEG;
    R_Bcd[1] = Shuzi_NEG;
    R_Bcd[0] = Shuzi_NEG;
}
// BCD 空显示
void display_bcd_null(void)
{
    R_Bcd[5] = Shuzi_NULL;
    R_Bcd[4] = Shuzi_NULL;
    R_Bcd[3] = Shuzi_NULL;
    R_Bcd[2] = Shuzi_NULL;
    R_Bcd[1] = Shuzi_NULL;
    R_Bcd[0] = Shuzi_NULL;
}
