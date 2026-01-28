#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

// 标准库头文件
#include <stdio.h>
#include <string.h>
#include <math.h>

// ESP-IDF 头文件
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_wifi_types.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
// #include "sdkconfig.h"

// 自定义头文件
#include "BSP/bsp_log.h"
#include "BSP/SPI/bsp_soft_spi.h"
#include "BSP/SPI/bsp_spi2.h"
#include "BSP/GPL811X.h"
#include "BSP/bsp_key.h"
#include "BSP/bsp_lcd.h"
#include "bsp_wifi.h"
#include "LCD/ws3900_lcd.h"
#include "LCD/lcd_display.h"

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
// 16位字访问联合体
typedef union
{
    uint16_t Val;
    struct
    {
        uint8_t Byte0; // 0-7
        uint8_t Byte1; // 8-15
    } Byte;
    struct
    {
        uint8_t Bit0 : 1;
        uint8_t Bit1 : 1;
        uint8_t Bit2 : 1;
        uint8_t Bit3 : 1;
        uint8_t Bit4 : 1;
        uint8_t Bit5 : 1;
        uint8_t Bit6 : 1;
        uint8_t Bit7 : 1;
        uint8_t Bit8 : 1;
        uint8_t Bit9 : 1;
        uint8_t Bit10 : 1;
        uint8_t Bit11 : 1;
        uint8_t Bit12 : 1;
        uint8_t Bit13 : 1;
        uint8_t Bit14 : 1;
        uint8_t Bit15 : 1;
    } Bit;
} Word;
extern struct model_info gw_model;
#endif // BSP_CONFIG