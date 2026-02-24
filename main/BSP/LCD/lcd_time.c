#include "lcd_time.h"

// 全局变量
uint8_t time_format = eTime_12H; // 时间格式，默认12小时制

// 24小时制转12小时制
// 得到结果存储在全局变量：R_Bcd[0] R_Bcd[1]
void lcd_24To12(uint8_t hour)
{
    if (eTime_12H != time_format)
    {
        R_Bcd[1] = 0; // 0:AM
        R_Bcd[0] = hour;
        return;
    }
    R_Bcd[1] = (hour >= 12) ? 1 : 0; // 0:AM 1:PM
    if (hour >= 12)
        R_Bcd[0] = hour - 12;
    else
        R_Bcd[0] = (hour == 0) ? 12 : hour;
}

// 显示小时
void display_hour(uint8_t hour, uint8_t flash)
{
    if (flash) // 是否闪烁显示
    {
        lcd_24To12(hour);
        F_DisplayDot(_T_PM, R_Bcd[1]); // 显示PM标志
        hour = R_Bcd[0];
        F_Change2BCD(hour); //
    }
    else
        F_NULLBCD();

    F_DisplayDot(_S166_L4, (R_Bcd[1] == 2));                    // 十位为2时点亮
    F_DisplayDot(_S167_L4, (R_Bcd[1] == 2 || (R_Bcd[1] == 1))); // 十位为1或2时点亮
    F_DisplayDot(_S168_L4, (R_Bcd[1] == 1));                    // 十位为1时点亮
    F_DisplayShuzi(number34_L4, R_Bcd[0]);                      // 显示个位
}
// 分钟显示
void display_minute(uint8_t minute, uint8_t flash)
{
    if (flash)
    {
        F_Change2BCD(minute);
    }
    else
    {
        F_NULLBCD();
    }
    F_DisplayShuzi(number3, R_Bcd[1]);
    F_DisplayShuzi(number4, R_Bcd[0]);
}
// 根据12H/24H格式显示小时
void display_set12or24(void)
{
    if (time_format == eTime_12H)
    {
        display_hour(12, B_1HzFlash || B_IncDec);
    }
    else
    {
        display_hour(24, B_1HzFlash || B_IncDec);
    }

    F_DisplayShuzi(number36_L4, Shuzi_H);
}