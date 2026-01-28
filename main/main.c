#include "BSP/bsp_config.h"

void app_main(void)
{
     //==== 初始化 WiFi 模块 ====
     esp_err_t ret = nvs_flash_init(); // 初始化 NVS(WIFI 等模块必需)
     // 如果 NVS 分区被擦除过，重新初始化
     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
     {
          ESP_ERROR_CHECK(nvs_flash_erase());
          ret = nvs_flash_init();
     }
     ESP_ERROR_CHECK(ret); // 确认 NVS 初始化成功
     wifi_init();          // 初始化 WiFi 模块

     //==== 初始化 SPI 总线 ====
     init_spi_mutex();          // 初始化 SPI 互斥量
     soft_spi_init();           //  初始化软件 SPI
     if (!soft_spi_test_comm()) //  测试通信
     {
          ESP_LOGE("", "SPI 通信失败!");
          return;
     }
     //==== 初始化 LCD 显示屏 ====
     lcd_init(); // 初始化 LCD 控制器
     // 初始化并点亮背光
     backlight_init();
     backlight_set(6); // 亮度 6/7
     // LCD 全显示
     lcd_full_on();
     //==== 初始化按键扫描 ====
     key_init();

     while (1)
     {
          vTaskDelay(pdMS_TO_TICKS(1000));
     }
}