#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

#include "BSP/bsp_log.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define set(val, bit) (val |= (bit))    // 置1
#define clr(val, bit) (val &= (~(bit))) // 清0
#define tst(val, bit) (val & (bit))     // 判断是否为1
#define tog(val, bit) (val ^= (bit))    // 取反

struct lcd_t
{
    enum
    {
        eLCD_TYPE_NONE = 0,
        eLCD_TYPE_WS1820,
        eLCD_TYPE_WS3900,
    } type; // LCD 类型
}; // LCD属性结构体
struct key
{
};

struct model_info
{
    struct lcd_t screen; // 屏幕类型
    u8 key;
};

extern struct model_info gw_model;
#endif // BSP_CONFIG