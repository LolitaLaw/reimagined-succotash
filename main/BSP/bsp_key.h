#ifndef _BSP_KEY_H_
#define _BSP_KEY_H_

#include "bsp_config.h"

// ================= 配置参数 =================
#define KEY_SCAN_INTERVAL_MS 20 // 扫描周期 20ms
#define DEBOUNCE_TICKS 2        // 消抖时间 = 2 * 20ms = 40ms
#define LONG_PRESS_TIME 100     // 长按阈值 = 100 * 20ms = 2000ms (2秒)
#define REPEAT_RATE_TICKS 10    // 长按连发速率 = 10 * 20ms = 200ms
#define KEY_QUEUE_LEN 10        // 队列深度

// ================= 按键 ID =================
typedef enum
{
    KEYS_NONE = 0,
    KEYS_SET, // SET键
    KEYS_TEMP,
    KEYS_RAIN1, // RAIN1键
    KEYS_RAIN2, // RAIN2键
    KEYS_WIND,
    KEYS_UP,    // +键
    KEYS_DOWN,  // -键
    KEYS_LIGHT, // LIGHT键
    KEY_COUNT
} key_id_t;

// ================= 按键事件 =================
typedef enum
{
    KEYS_EVT_NONE = 0,
    KEYS_EVT_PRESS,       // 短按按下
    KEYS_EVT_RELEASE,     // 短按抬起（通常作为点击事件）
    KEYS_EVT_LONG_PRESS,  // 长按触发
    KEYS_EVT_LONG_RELEASE // 长按后抬起
} key_event_t;

// ================= 状态机定义 =================
typedef enum
{
    STATE_IDLE = 0,   // 空闲
    STATE_DEBOUNCE,   // 消抖中
    STATE_PRESS,      // 按下确认
    STATE_LONG_PRESS, // 长按确认
    // STATE_RELEASE_WAIT // 等待松开
} key_state_t;
// ================= 按键配置结构体 =================
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

// ================= 按键消息结构体 =================
typedef struct
{
    key_id_t id;       // 按键ID
    key_event_t event; // 按键事件
} key_msg_t;

// ================= 初始化函数 =================
void key_init();
// 获取按键消息 (Main函数调用此函数等待按键)
// timeout: 等待时间，可以是 0 (不等待) 或 portMAX_DELAY (死等)
// 返回值: true 表示获取到了按键，false 表示超时没按键
bool bsp_key_get_event(key_msg_t *msg, TickType_t timeout);
#endif