#ifndef _BSP_KEY_H_
#define _BSP_KEY_H_

#include <stdint.h>
#include "GPL811X.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp/spi/bsp_spi2.h"

// ================= 配置参数 =================
#define KEY_SCAN_INTERVAL_MS 20 // 扫描周期 20ms
#define DEBOUNCE_TICKS 2        // 消抖时间 = 2 * 20ms = 40ms
#define LONG_PRESS_TICKS 100    // 长按阈值 = 100 * 20ms = 2000ms (2秒)
#define REPEAT_RATE_TICKS 10    // 长按连发速率 = 10 * 20ms = 200ms

// ================= 按键 ID =================
typedef enum
{
    KEYS_NONE = 0,
    KEYS_SET, // SET键
    KEYS_TEMP,
    KEYS_RAIN1, // RAIN1键
    KEYS_RAIN2, // RAIN2键
    KEYS_WIND,
    KEYS_DOWN,  // -键
    KEYS_UP,    // +键
    KEYS_LIGHT, // LIGHT键
} key_id_t;

// ================= 按键事件 =================
typedef enum
{
    KEYS_EVT_PRESS,      // 按下瞬间 (通常用于唤醒屏幕)
    KEYS_EVT_SHORT,      // 短按释放 (Click)
    KEYS_EVT_LONG_START, // 长按触发 (Hold 2s)
    KEYS_EVT_LONG_HOLD,  // 长按保持 (每隔一段时间触发一次，用于连续加减)
    KEYS_EVT_RELEASE     // 松开 (无论长短)
} key_evt_t;

// ================= 状态机定义 =================
typedef enum
{
    STATE_IDLE = 0,  // 空闲
    STATE_DEBOUNCE,  // 消抖中
    STATE_PRESSED,   // 确认按下，等待释放或长按
    STATE_LONG_PRESS // 进入长按状态
} key_state_t;
typedef struct
{
    // --- 硬件属性 (只读) ---
    key_id_t id;        // 逻辑ID (KEYS_SET, KEYS_UP...)
    uint16_t port_addr; // 寄存器地址 (P_IO_PortE_Data 或 P_IO_PortD_Data)
    uint8_t pin_mask;   // 位掩码 (如 0x04, 0x08)

    // --- 软件状态 (动态) ---
    key_state_t state; // 当前状态
    uint16_t ticks;    // 计时器
} key_cfg_t;
// ================= 回调函数原型 =================
typedef void (*key_callback_t)(key_id_t key, key_evt_t evt);

// ================= 初始化函数 =================
void key_init(key_callback_t cb);

#endif