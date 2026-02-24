#ifndef __BSP_WIFI_H__
#define __BSP_WIFI_H__

#include "BSP/bsp_config.h"

#define WIFI_SSID "TP-LINK_D1BE"         // 要连接的路由器
#define WIFI_PASSWORD "11223344"         // 路由器密码

#define WIFI_AP_SSID "M"      // 热点名
#define WIFI_AP_PASSWORD "esp32ap123" // 热点密码
#define WIFI_AP_CHANNEL 1             // 热点频道
#define WIFI_AP_MAX_CONN 4            // 最大连接数

void wifi_init(void);
#endif // BSP_WIFI