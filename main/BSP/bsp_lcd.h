#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

#include "driver/ledc.h"
#include "esp_log.h"
#include "BSP/SPI/bsp_soft_spi.h"
#include "BSP/SPI/bsp_spi2.h"
#include "BSP/bsp_key.h"
#include "BSP/GPL811X.h"
#include "LCD/ws3900_lcd.h"

void lcd_full_on();
void lcd_clear();
void lcd_display(uint16_t addr, bool state);
void backlight_set(uint8_t level);
void backlight_init();
#endif // BSP_LCD