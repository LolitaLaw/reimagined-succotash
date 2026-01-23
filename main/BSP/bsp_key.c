#include "bsp_key.h"

// ================= 按键列表 (8个按键的硬件映射) =================
// 注意：请根据你的原理图确认 Port 和 Mask
static key_cfg_t g_keys[] = {
    {KEYS_SET, P_IO_PortE_Data, D_IOE_7_KeyScan, STATE_IDLE, 0},   //* PortE Bit 7
    {KEYS_TEMP, P_IO_PortE_Data, D_IOE_6_KeyScan, STATE_IDLE, 0},  //* PortE Bit 6
    {KEYS_RAIN1, P_IO_PortD_Data, D_IOD_3_KeyScan, STATE_IDLE, 0}, //! PortD Bit 3
    {KEYS_RAIN2, P_IO_PortD_Data, D_IOD_4_KeyScan, STATE_IDLE, 0}, //! PortD Bit 4
    {KEYS_WIND, P_IO_PortE_Data, D_IOE_5_KeyScan, STATE_IDLE, 0},  //* PortE Bit 5
    {KEYS_DOWN, P_IO_PortE_Data, D_IOE_3_KeyScan, STATE_IDLE, 0},  //* PortE Bit 3
    {KEYS_UP, P_IO_PortE_Data, D_IOE_4_KeyScan, STATE_IDLE, 0},    //* PortE Bit 4
    {KEYS_LIGHT, P_IO_PortE_Data, D_IOE_2_KeyScan, STATE_IDLE, 0}, //* PortE Bit 2
};
#define KEY_COUNT (sizeof(g_keys) / sizeof(key_cfg_t))

// ================= 回调函数 =================
static key_callback_t g_app_cb = NULL;

// ================= 状态机函数 =================
static void key_state_handler(key_cfg_t *key, bool is_hw_active)
{
    switch (key->state)
    {
        // 进入空闲状态
    case STATE_IDLE:
        if (is_hw_active)
        {
            key->state = STATE_DEBOUNCE; // 进入消抖状态
            key->ticks = 0;
        }
        break;
        // 进入消抖状态
    case STATE_DEBOUNCE:
        if (is_hw_active)
        {
            key->ticks++;
            if (key->ticks >= DEBOUNCE_TICKS) // 达到消抖时间
            {
                key->state = STATE_PRESSED; // 消抖完成，确认为按下
                key->ticks = 0;
                // 事件：按下 (如果你需要在按下瞬间就响应，在这里回调)
                if (g_app_cb)
                    g_app_cb(key->id, KEYS_EVT_PRESS);
            }
        }
        else
            key->state = STATE_IDLE; // 还没达到消抖时间就松开了，视为干扰
        break;
        // 按下状态，等待释放或长按
    case STATE_PRESSED:
        if (is_hw_active)
        {
            // 持续按下，累计时间
            key->ticks++;
            if (key->ticks >= LONG_PRESS_TICKS)
            {
                // 达到长按阈值
                key->state = STATE_LONG_PRESS;
                if (g_app_cb)
                    g_app_cb(key->id, KEYS_EVT_LONG_START);
            }
        }
        else
        {
            // 在长按触发前松开 -> 判定为“短按/单击”
            if (g_app_cb)
                g_app_cb(key->id, KEYS_EVT_SHORT);
            // 同时触发通用的抬起事件
            if (g_app_cb)
                g_app_cb(key->id, KEYS_EVT_RELEASE);
            key->state = STATE_IDLE;
        }
        break;

    case STATE_LONG_PRESS:
        if (is_hw_active)
        {
            // 长按保持中... (可选：实现连发功能)
            // if (g_app_cb) g_app_cb(key->id, KEYS_EVT_LONG_HOLD);
        }
        else
        {
            // 长按后松开
            if (g_app_cb)
                g_app_cb(key->id, KEYS_EVT_RELEASE); // 触发抬起
            key->state = STATE_IDLE;
        }
        break;
    }
}

// ================= 1. 物理层：硬件读取与映射 =================
// 作用：屏蔽硬件细节，处理组合键优先级
static key_id_t key_hal_read(void)
{
    // 1. 读取 GPL811X 的 Port E 和 Port D (根据你的原理图调整)
    // 假设 user_spi.c 中已经处理了互斥锁，这里直接读
    uint8_t raw_e = SPI2_ReadByte(P_IO_PortE_Data);
    uint8_t raw_d = SPI2_ReadByte(P_IO_PortD_Data);

    // 低电平有效，按下时对应位为 0
    // 如果是高电平有效，则取反 ~
    // 这里为了演示，假设按键都在 PortE，你需要根据 D_IOE_x_KeyScan 宏修改掩码

    // 2. 优先判断组合键 (防止组合键被误判为单键)
    // 示例：假设 Bit0 是 SET, Bit1 是 UP
    // bool b_set = (raw_e & 0x04); // 假设0是按下
    // bool b_up = (raw_e & 0x08);
    // bool b_down = (raw_e & 0x10);
    // bool pin0_E = (raw_e & D_IOE_0_KeyScan);
    // bool pin1_E = (raw_e & D_IOE_1_KeyScan);

    // bool pin0_D = (raw_d & D_IOD_0_KeyScan);
    // bool pin1_D = (raw_d & D_IOD_1_KeyScan);
    // bool pin2_D = (raw_d & D_IOD_2_KeyScan);
    // bool pin3_D = (raw_d & D_IOD_3_KeyScan);
    // bool pin4_D = (raw_d & D_IOD_4_KeyScan);
    // bool pin5_D = (raw_d & D_IOD_5_KeyScan);

    // if (b_set && b_up)
    //     return KEYS_COMBO_SET_UP;
    // if (b_up && b_down)
    //     return KEYS_COMBO_UP_DOWN;

    // // 3. 判断单键
    // if (b_set)
    //     return KEYS_SET;
    // if (b_up)
    //     return KEYS_UP;
    // if (b_down)
    //     return KEYS_DOWN;
    return KEYS_NONE;
}

// ================= 2. 核心层：状态机处理 =================
static void key_process_task(void *arg)
{
    while (1)
    {
        // 1. 批量读取硬件状态 (只读一次 SPI，效率高)
        // 注意：这里假设 init_spi_mutex 已经初始化
        uint8_t raw_e = SPI2_ReadByte(P_IO_PortE_Data);
        uint8_t raw_d = SPI2_ReadByte(P_IO_PortD_Data);

        // 2. 遍历所有按键进行处理
        for (int i = 0; i < KEY_COUNT; i++)
        {
            key_cfg_t *k = &g_keys[i];
            bool is_active = false;

            // 根据该按键所在的 Port 提取对应的位
            if (k->port_addr == P_IO_PortE_Data)
            {
                // 假设高电平有效 (Active High)
                is_active = (raw_e & k->pin_mask) ? true : false;
            }
            else if (k->port_addr == P_IO_PortD_Data)
            {
                is_active = (raw_d & k->pin_mask) ? true : false;
            }

            // 3. 运行该按键的状态机
            key_state_handler(k, is_active);
        }

        // 4. 延时 (20ms 扫描一次)
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
static void key_process_task2(void *arg)
{
    static key_state_t state = STATE_IDLE;
    static key_id_t active_key = KEYS_NONE;
    static uint32_t ticks = 0;

    while (1)
    {
        // 1. 获取当前逻辑键值
        key_id_t curr_key = key_hal_read();

        // 2. 状态机流转
        switch (state)
        {
        case STATE_IDLE:
            if (curr_key != KEYS_NONE)
            {
                state = STATE_DEBOUNCE; // 检测到按下，去消抖
                active_key = curr_key;  // 锁定当前按键
                ticks = 0;
            }
            break;

        case STATE_DEBOUNCE:
            ticks++;
            if (ticks >= DEBOUNCE_TICKS)
            {
                if (curr_key == active_key)
                {
                    // 消抖通过，确认为按下
                    state = STATE_PRESSED;
                    ticks = 0;
                    if (g_app_cb)
                        g_app_cb(active_key, KEYS_EVT_PRESS);
                }
                else
                {
                    // 抖动或者是干扰，回到空闲
                    state = STATE_IDLE;
                }
            }
            break;

        case STATE_PRESSED:
            if (curr_key != active_key)
            {
                // 按键释放 (或者是换了别的键) -> 触发短按
                if (g_app_cb)
                {
                    g_app_cb(active_key, KEYS_EVT_SHORT);
                    g_app_cb(active_key, KEYS_EVT_RELEASE);
                }
                state = STATE_IDLE;
            }
            else
            {
                // 按键一直按着 -> 计时
                ticks++;
                if (ticks >= LONG_PRESS_TICKS)
                {
                    // 达到长按时间 -> 触发长按开始
                    state = STATE_LONG_PRESS;
                    ticks = 0; // 重置用于连发计时
                    if (g_app_cb)
                        g_app_cb(active_key, KEYS_EVT_LONG_START);
                }
            }
            break;

        case STATE_LONG_PRESS:
            if (curr_key != active_key)
            {
                // 长按后松开
                if (g_app_cb)
                    g_app_cb(active_key, KEYS_EVT_RELEASE);
                state = STATE_IDLE;
            }
            else
            {
                // 长按保持中 -> 处理连发 (Repeat)
                ticks++;
                if (ticks >= REPEAT_RATE_TICKS)
                {
                    ticks = 0;
                    if (g_app_cb)
                        g_app_cb(active_key, KEYS_EVT_LONG_HOLD);
                }
            }
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(KEY_SCAN_INTERVAL_MS));
    }
}

// ================= 3. 初始化接口 =================
void key_init(key_callback_t cb)
{
    g_app_cb = cb;

    // 创建 FreeRTOS 任务来跑状态机
    // 栈大小 2048, 优先级设适中
    xTaskCreate(key_process_task, "key_task", 2048, NULL, 5, NULL);

    ESP_LOGI("KEY", "按键初始化完成");
}