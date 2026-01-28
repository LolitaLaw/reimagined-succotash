#include "bsp_lcd.h"
static const char *TAG = "LCD";

// 背光引脚
#define BACKLED_GPIO GPIO_NUM_3

// LCD RAM
#define LCD_RAM_LENGTH 64
#define LCD_RAM_ADDR 0x3E00

static uint8_t lcd_ram[LCD_RAM_LENGTH + 1];

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
}

void backlight_set(uint8_t level)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, level);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

/*============================================================================
 *                              LCD 控制
 *===========================================================================*/

void lcd_update(void)
{
    static uint8_t lcd_cache[LCD_RAM_LENGTH];
    uint16_t addr = LCD_RAM_ADDR;

    for (int i = 0; i < LCD_RAM_LENGTH / 8; i++)
    {
        if (memcmp(&lcd_cache[i * 8], &lcd_ram[i * 8], 8) != 0)
        {
            soft_spi_write_nbytes(addr, &lcd_ram[i * 8], 8);
            memcpy(&lcd_cache[i * 8], &lcd_ram[i * 8], 8);
        }
        addr += 16;
    }
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
    {
        set(lcd_ram[addr_offset], bit_offset);
    }
    else
    {
        clr(lcd_ram[addr_offset], bit_offset);
    }
}