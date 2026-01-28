#include "bsp_key.h"
#include "bsp_lcd.h"

// --- 全局变量 ---
static QueueHandle_t g_key_queue = NULL;

// ================= 按键列表 (8个按键的硬件映射) =================
// 注意：请根据你的原理图确认 Port 和 Mask
static key_cfg_t g_keys[] = {
    // ID,          Port,             Mask,       State,   Tick
    {KEYS_SET, P_IO_PortE_Data, D_IOE_7_KeyScan, STATE_IDLE, 0},   //* PortE Bit 7
    {KEYS_TEMP, P_IO_PortE_Data, D_IOE_6_KeyScan, STATE_IDLE, 0},  //* PortE Bit 6
    {KEYS_RAIN1, P_IO_PortD_Data, D_IOD_3_KeyScan, STATE_IDLE, 0}, //! PortD Bit 3
    {KEYS_RAIN2, P_IO_PortD_Data, D_IOD_4_KeyScan, STATE_IDLE, 0}, //! PortD Bit 4
    {KEYS_WIND, P_IO_PortE_Data, D_IOE_5_KeyScan, STATE_IDLE, 0},  //* PortE Bit 5
    {KEYS_DOWN, P_IO_PortE_Data, D_IOE_3_KeyScan, STATE_IDLE, 0},  //* PortE Bit 3
    {KEYS_UP, P_IO_PortE_Data, D_IOE_4_KeyScan, STATE_IDLE, 0},    //* PortE Bit 4
    {KEYS_LIGHT, P_IO_PortE_Data, D_IOE_2_KeyScan, STATE_IDLE, 0}, //* PortE Bit 2
};

// --- 内部辅助函数：发送事件到队列 ---
static void send_key_event(key_id_t id, key_event_t evt)
{
    if (g_key_queue)
    {
        key_msg_t msg;
        msg.id = id;
        msg.event = evt;
        xQueueSend(g_key_queue, &msg, 0); // 发送消息，如果队列满了就丢弃，不等待
    }
}

// --- 任务主体 ---
// 生产者任务：扫描按键状态，更新状态机
// 生产出key_msg_t消息到队列
static void key_process_task(void *arg)
{
    while (1)
    {
        // 获取 SPI 互斥量
        SemaphoreHandle_t mutex = bsp_spi_get_lock();
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
        {
            // 1. 读取硬件 (保留原本的高效率读取方式)
            // 注意：请确保 mutex 已经在别处或 init 中初始化，或者在这里加锁
            uint8_t raw_e = SPI2_ReadByte(P_IO_PortE_Data); // 读取 PortE 数据
            uint8_t raw_d = SPI2_ReadByte(P_IO_PortD_Data); // 读取 PortD 数据

            // 释放互斥量
            xSemaphoreGive(mutex);
            // 2. 遍历处理所有按键
            for (int i = 0; i < KEY_COUNT; i++)
            {
                key_cfg_t *k = &g_keys[i];
                bool is_active = false;
                // 提取电平状态
                if (k->port_addr == P_IO_PortE_Data)
                    is_active = (raw_e & k->pin_mask) ? true : false;
                else
                    is_active = (raw_d & k->pin_mask) ? true : false;

                // 3. 状态机逻辑
                switch (k->state)
                {
                case STATE_IDLE: // 空闲状态，等待按下
                    if (is_active)
                    {
                        k->state = STATE_DEBOUNCE;
                        k->ticks = 0;
                    }
                    break;
                case STATE_DEBOUNCE: // 消抖状态，等待稳定按下
                    if (is_active)
                    {
                        k->ticks++;
                        if (k->ticks >= 2)
                        { // 2 * 20ms = 40ms 消抖
                            k->state = STATE_PRESS;
                            k->ticks = 0;
                            // 触发按下事件
                            send_key_event(k->id, KEYS_EVT_PRESS);
                        }
                    }
                    else
                        k->state = STATE_IDLE; // 抖动，回去
                    break;
                case STATE_PRESS: // 按下状态，等待释放或长按
                    if (is_active)
                    {
                        k->ticks++;
                        if (k->ticks >= LONG_PRESS_TIME)
                        {
                            k->state = STATE_LONG_PRESS;
                            // 触发长按事件
                            send_key_event(k->id, KEYS_EVT_LONG_PRESS);
                        }
                    }
                    else
                    {
                        // 没到长按时间就松开了 -> 短按释放 (Click)
                        send_key_event(k->id, KEYS_EVT_RELEASE);
                        k->state = STATE_IDLE;
                    }
                    break;
                case STATE_LONG_PRESS: // 长按状态，等待释放
                    if (!is_active)
                    {
                        // 长按结束
                        send_key_event(k->id, KEYS_EVT_LONG_RELEASE);
                        k->state = STATE_IDLE;
                    }
                    break;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(KEY_SCAN_INTERVAL_MS));
    }
}

/*
 * ======================================================
 * ================= 应用层按键业务处理 =================
 * ======================================================
 */
// SET键处理
static void handle_set_key(key_event_t event)
{
    if (event == KEYS_EVT_RELEASE)
    {
        ESP_LOGI("", "operation set");
        lcd_display(_T_Time, 1);
        lcd_display(_T_PM, 1);
        lcd_display(_T_dst, 1);
        lcd_display(_T_horn, 1);
        lcd_display(_T_buzz, 1);
        lcd_display(_T_Wifi0, 1);
        lcd_display(_T_moon1, 1);
        lcd_display(_T_moon, 1);
    }
    else if (event == KEYS_EVT_LONG_PRESS)
    {
        ESP_LOGI("", "operation long press set");
        lcd_display(_T_Time, 0);
        lcd_display(_T_PM, 0);
        lcd_display(_T_dst, 0);
        lcd_display(_T_horn, 0);
        lcd_display(_T_buzz, 0);
        lcd_display(_T_Wifi0, 0);
        lcd_display(_T_moon1, 0);
        lcd_display(_T_moon, 0);
    }
}
// TEMP键处理
static void handle_tmpe_key(key_event_t event)
{
    if (event == KEYS_EVT_RELEASE)
    {
        ESP_LOGI("", "operation temp");
    }
}
// RAIN1键处理
static void handle_rain1_key(key_event_t event)
{
    if (event == KEYS_EVT_RELEASE)
    {
        ESP_LOGI("", "operation rain1");
    }
    else if (event == KEYS_EVT_LONG_PRESS)
    {
        ESP_LOGI("", "operation long press rain1");
    }
}
// RAIN2键处理
static void handle_rain2_key(key_event_t event)
{
    if (event == KEYS_EVT_RELEASE)
    {
        ESP_LOGI("", "operation rain2");
    }
    else if (event == KEYS_EVT_LONG_PRESS)
    {
        ESP_LOGI("", "operation long press rain2");
    }
}
// WIND键处理
static void handle_wind_key(key_event_t event)
{
    if (event == KEYS_EVT_RELEASE)
    {
        ESP_LOGI("", "operation wind");
    }
}
// UP键处理
static void handle_up_key(key_event_t event)
{
    if (event == KEYS_EVT_RELEASE)
    {
        ESP_LOGI("", "operation up");
    }
}
// DOWN键处理
static void handle_down_key(key_event_t event)
{
    if (event == KEYS_EVT_RELEASE)
    {
        ESP_LOGI("", "operation down");
    }
}
// LIGHT键处理
static void handle_light_key(key_event_t event)
{
    static u8 level = 0;
    static u8 backlight_on = 0;
    static bool light_on = true;
    if (event == KEYS_EVT_RELEASE)
    {
        // ESP_LOGI("", "operation light");
        ESP_LOGI("", "Level%d: %d", level, backlight_on);
        backlight_set(backlight_on);
        if (backlight_on <= 8)
        {
            backlight_on += pow(2, ++level) - 1;
        }
        else
            backlight_on = level = 0;
    }
    else if (event == KEYS_EVT_LONG_PRESS)
    {
        ESP_LOGI("", "operation long press light");
        light_on = !light_on;
        if (light_on)
            lcd_full_on();
        else
            lcd_clear();
    }
}
// 获取按键消息
bool bsp_key_get_event(key_msg_t *msg, TickType_t timeout)
{
    if (g_key_queue == NULL)
        return false;
    return (xQueueReceive(g_key_queue, msg, timeout) == pdTRUE); // 从队列取数据
}

// 消费者任务：调用xQueueReceive等待按键消息，并分发处理
static void app_key_task(void *arg)
{
    key_msg_t msg;
    ESP_LOGI("", "Key business task started...");
    while (1)
    {
        // 阻塞等待 BSP 层发来的消息
        if (bsp_key_get_event(&msg, portMAX_DELAY))
        {
            ESP_LOGD("", "收到按键 ID: %d, 事件: %d", msg.id, msg.event);
            // 集中分发处理
            switch (msg.id)
            {
            case KEYS_SET:
                handle_set_key(msg.event);
                break;
            case KEYS_TEMP:
                handle_tmpe_key(msg.event);
                break;
            case KEYS_RAIN1:
                handle_rain1_key(msg.event);
                break;
            case KEYS_RAIN2:
                handle_rain2_key(msg.event);
                break;
            case KEYS_WIND:
                handle_wind_key(msg.event);
                break;
            case KEYS_UP:
                handle_up_key(msg.event);
                break;
            case KEYS_DOWN:
                handle_down_key(msg.event);
                break;
            case KEYS_LIGHT:
                handle_light_key(msg.event);
                break;
            default:
                // 其他按键暂无业务逻辑
                break;
            }
        }
    }
}

// --- 公共初始化接口 ---

void key_init(void)
{
    // 1. 创建队列
    g_key_queue = xQueueCreate(KEY_QUEUE_LEN, sizeof(key_msg_t));
    // 2. 可以在这里初始化 SPI (如果 SPI 还没被别的模块初始化)
    // bsp_spi_init();
    xTaskCreate(key_process_task, "KeyTask", 2048, NULL, 5, NULL); // 3. 创建按键扫描任务
    // 优先级建议比 bsp_key_task 低一点点，或者相同
    xTaskCreate(app_key_task, "AppKeyTask", 4096, NULL, 4, NULL); // 4. 创建按键业务处理任务
}
