#include "bsp_uart.h"
void uart_init(void)
{
    // 创建UART配置结构体
    uart_config_t uart_config = {
        .baud_rate = 115200,                   // 配置115200波特率
        .data_bits = UART_DATA_8_BITS,         // 配置8位数据位
        .parity = UART_PARITY_DISABLE,         // 配置校验位
        .stop_bits = UART_STOP_BITS_1,         // 配置停止位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // 配置流控
        .source_clk = UART_SCLK_APB,           // 配置时钟源
    };

    // 配置UART参数
    uart_param_config(UART_NUM, &uart_config);

    // 设置UART引脚（TX, RX, RTS, CTS）
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // 安装UART驱动程序（使用事件队列）
    uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);
}

// 串口发送函数
void uart_send_data(const char *data)
{
    uart_write_bytes(UART_NUM, data, strlen(data));
}

// 串口接收任务 - 实时回显版本
void uart_receive_task(void *arg)
{
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE); // 分配缓冲区
    int buffer_pos = 0;                          // 缓冲区当前位置

    while (1)
    {
        // 读取数据，等待时间短以实现实时响应
        int len = uart_read_bytes(UART_NUM, data + buffer_pos, 1, pdMS_TO_TICKS(20));

        if (len > 0)
        {
            buffer_pos += len;

            // 检查是否收到回车或换行符（表示一行结束）
            if (data[buffer_pos - 1] == '\r' || data[buffer_pos - 1] == '\n')
            {
                // 添加字符串结束符
                data[buffer_pos] = '\0';
                // 打印接收到的数据（去掉回车换行）
                if (buffer_pos > 1)
                {
                    data[buffer_pos - 1] = '\0'; // 去掉\r或\n
                    // printf("接收: %s\n", data);
                    // 回显收到的数据
                    uart_write_bytes(UART_NUM, (const char *)data, buffer_pos - 1);
                    uart_send_data("\r\n");
                }

                // 清空缓冲区
                buffer_pos = 0;
            }
            else if (buffer_pos >= BUF_SIZE - 1)
            {
                // 缓冲区满，强制处理
                data[buffer_pos] = '\0';
                printf("接收(缓冲区满): %s\n", data);
                uart_write_bytes(UART_NUM, (const char *)data, buffer_pos);
                uart_send_data("\r\n");
                buffer_pos = 0;
            }
        }
    }

    free(data);
}
