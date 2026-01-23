#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "BSP/spi/bsp_soft_spi.h"
#include "BSP/bsp_key.h"
#include "BSP/spi/bsp_spi2.h"
#include "BSP/GPL811X.h"

static const char *TAG = "LCD";

// 背光引脚
#define BACKLED_GPIO GPIO_NUM_3

// LCD RAM
#define LCD_RAM_LENGTH 64
#define LCD_RAM_ADDR 0x3E00

static uint8_t lcd_ram[LCD_RAM_LENGTH];

static void lcd_full_on();
static void lcd_clear();
static void backlight_set(uint8_t level);
/*============================================================================
 *                              按键处理
 *===========================================================================*/
static bool is_set_pressed = false;
void on_key_event(key_id_t key, key_evt_t evt)
{
     static u8 backlight_on = 1;
     static bool light_on = true;
     // ---------------------------------------------------------
     // SET全局状态追踪 (用于辅助组合键判断)
     // ---------------------------------------------------------
     if (key == KEYS_SET)
     {
          if (evt == KEYS_EVT_PRESS)
          {
               is_set_pressed = true; // SET 按下
               ESP_LOGI("", "SET down");
          }
          else if (evt == KEYS_EVT_RELEASE)
          {
               is_set_pressed = false; // SET 抬起
               ESP_LOGI("", "SET up");
          }
     }

     switch (key)
     {
     case KEYS_SET:
          if (evt == KEYS_EVT_LONG_START) // 长按2秒
          {
               ESP_LOGI("", "SET long press");
               // 函数调用();
          }
          else if (evt == KEYS_EVT_SHORT) // 短按
          {
               ESP_LOGI("", "SET short press");
          }
          break;

     case KEYS_TEMP:
          if (evt == KEYS_EVT_SHORT)
          {
               ESP_LOGI("", "TEMP short press");
          }
          else if (evt == KEYS_EVT_LONG_START)
          {
               ESP_LOGI("", "TEMP long press");
          }
          break;

     // === 数值调节键 (带组合功能) ===
     case KEYS_DOWN:
          if (evt == KEYS_EVT_SHORT) // 支持点按和长按连发
          {
               ESP_LOGI("", "DOWN short press - 数值 -1");
          }
          else if (evt == KEYS_EVT_LONG_HOLD)
          {
               if (is_set_pressed)
               {
                    // [组合键逻辑] SET 按住的同时按 DOWN
                    ESP_LOGI("", "SET + DOWN - 触发工厂复位");
                    // factory_reset();
               }
               else
               {
                    ESP_LOGI("", "DOWN long hold - 数值 -10");
               }
          }

          break;

     case KEYS_UP:
          if (evt == KEYS_EVT_SHORT) // 支持点按和长按连发
          {
               ESP_LOGI("", "UP short press - 数值 +1");
          }
          else if (evt == KEYS_EVT_LONG_HOLD)
          {
               if (is_set_pressed)
               {
                    // [组合键逻辑] SET 按住的同时按 UP
                    ESP_LOGI("", "【组合键】SET + UP - 触发工厂复位");
                    // factory_reset();
               }
               else
               {
                    ESP_LOGI("", "UP long hold - 数值 +10");
               }
          }
          break;

     // === 功能键 ===
     case KEYS_LIGHT:
          if (evt == KEYS_EVT_SHORT)
          {
               switch (backlight_on)
               {
               case 0:
                    backlight_set(0);
                    backlight_on = 1;
                    ESP_LOGI("", "LIGHT short press - backlight off");
                    break;
               case 1:
                    backlight_set(1);
                    backlight_on = 2;
                    ESP_LOGI("", "LIGHT short press - level 1");
                    break;
               case 2:
                    backlight_set(2);
                    backlight_on = 3;
                    ESP_LOGI("", "LIGHT short press - level 2");
                    break;
               case 3:
                    backlight_set(4);
                    backlight_on = 4;
                    ESP_LOGI("", "LIGHT short press - level 4");
                    break;
               case 4:
                    backlight_set(8);
                    backlight_on = 0;
                    ESP_LOGI("", "LIGHT short press - level 8");
                    break;
               default:
                    break;
               }
          }
          else if (evt == KEYS_EVT_LONG_START)
          {
               ESP_LOGI("", "LIGHT long press");
               if (light_on)
               {
                    lcd_full_on();
                    light_on = false;
               }
               else
               {
                    lcd_clear();
                    light_on = true;
               }
          }
          break;

     case KEYS_RAIN1:
          if (evt == KEYS_EVT_SHORT)
          {
               ESP_LOGI("", "RAIN1 short press");
          }
          else if (evt == KEYS_EVT_LONG_START)
          {
               ESP_LOGI("", "RAIN1 long press");
          }
          break;

     case KEYS_RAIN2:
          if (evt == KEYS_EVT_SHORT)
          {
               ESP_LOGI("", "RAIN2 short press");
          }
          else if (evt == KEYS_EVT_LONG_START)
          {
               ESP_LOGI("", "RAIN2 long press");
          }

          break;

     case KEYS_WIND:
          if (evt == KEYS_EVT_SHORT)
          {
               ESP_LOGI("", "WIND short press");
          }
          else if (evt == KEYS_EVT_LONG_START)
          {
               ESP_LOGI("", "WIND long press");
          }
          break;
     default:
          break;
     }
}
/*============================================================================
 *                              背光控制
 *===========================================================================*/
static void backlight_init(void)
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

static void backlight_set(uint8_t level)
{
     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, level);
     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

/*============================================================================
 *                              LCD 控制
 *===========================================================================*/

static void lcd_update(void)
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

static void lcd_full_on(void)
{
     memset(lcd_ram, 0xFF, LCD_RAM_LENGTH);
     lcd_update();
     ESP_LOGI(TAG, "LCD 全显示");
}

static void lcd_clear(void)
{
     memset(lcd_ram, 0x00, LCD_RAM_LENGTH);
     lcd_update();
     ESP_LOGI(TAG, "LCD 清屏");
}

void app_main(void)
{
     ESP_LOGI(TAG, "======== 开始初始化 ========");
     init_spi_mutex(); // 初始化 SPI 互斥量

     // 1. 初始化软件 SPI
     soft_spi_init();

     // 2. 测试通信
     if (!soft_spi_test_comm())
     {
          ESP_LOGE(TAG, "SPI 通信失败!");
          return;
     }

     // 3. 初始化 LCD 控制器
     lcd_init();

     // 4. 初始化并点亮背光
     backlight_init();
     backlight_set(6); // 亮度 6/7

     // 5. LCD 全显示
     lcd_full_on();

     ESP_LOGI(TAG, "======== 初始化完成 ========");

     key_init(on_key_event);
     while (1)
     {
          vTaskDelay(pdMS_TO_TICKS(1000));
     }
}