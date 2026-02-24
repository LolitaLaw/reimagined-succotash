#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

#include "bsp_config.h"

extern uint8_t R_Bcd[6]; // BCD结果存储: [个位, 十位, 百位, 千位, 万位, 符号]

// LCD控制函数
void lcd_init();    // LCD 初始化
void lcd_full_on(); // LCD 全显示
void lcd_clear();   // LCD 清屏
void lcd_update(void);

// 背光控制函数
void backlight_init();
void backlight_set(uint8_t level);

// 绘制函数
void lcd_display(uint16_t addr, bool state); // 设置 LCD 某地址某位的显示状态

#endif // BSP_LCD