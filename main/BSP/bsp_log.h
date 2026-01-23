#ifndef __BSP_LOG_H__
#define __BSP_LOG_H__

#include "esp_log.h"

/* 设置日志级别 */
#define LOG_LVL ESP_LOG_VERBOSE
/* 断言（Assert） */
#define LOG_A(tag, enable, args, ...)                                                        \
    do                                                                                       \
    {                                                                                        \
        if (LOG_LVL >= ESP_LOG_ERROR)                                                        \
        {                                                                                    \
            if (enable)                                                                      \
            {                                                                                \
                do                                                                           \
                {                                                                            \
                    ESP_LOGE(tag, "%s[%d]," args "", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
                } while (1);                                                                 \
            }                                                                                \
        }                                                                                    \
    } while (0)

/* 错误(Error) */
#define LOG_E(tag, enable, args, ...)                                                    \
    do                                                                                   \
    {                                                                                    \
        if (LOG_LVL >= ESP_LOG_ERROR)                                                    \
        {                                                                                \
            if (enable)                                                                  \
            {                                                                            \
                ESP_LOGE(tag, "%s[%d] " args "", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            }                                                                            \
        }                                                                                \
    } while (0)

/* 警告(Warn) */
#define LOG_W(tag, enable, args, ...)                                                   \
    do                                                                                  \
    {                                                                                   \
        if (LOG_LVL >= ESP_LOG_WARN)                                                    \
        {                                                                               \
            if (enable)                                                                 \
            {                                                                           \
                ESP_LOGW(tag, "%s[%d]" args "", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            }                                                                           \
        }                                                                               \
    } while (0)

/* 信息(Info) */
#define LOG_I(tag, enable, args, ...)                  \
    do                                                 \
    {                                                  \
        if (LOG_LVL >= ESP_LOG_INFO)                   \
        {                                              \
            if (enable)                                \
            {                                          \
                ESP_LOGI(tag, args "", ##__VA_ARGS__); \
            }                                          \
        }                                              \
    } while (0)

/* 调试(Debug) */
#define LOG_D(tag, enable, args, ...)                  \
    do                                                 \
    {                                                  \
        if (LOG_LVL >= ESP_LOG_DEBUG)                  \
        {                                              \
            if (enable)                                \
            {                                          \
                ESP_LOGD(tag, args "", ##__VA_ARGS__); \
            }                                          \
        }                                              \
    } while (0)

/* 详细(Verbose) */
#define LOG_V(tag, enable, args, ...)                  \
    do                                                 \
    {                                                  \
        if (LOG_LVL >= ESP_LOG_VERBOSE)                \
        {                                              \
            if (enable)                                \
            {                                          \
                ESP_LOGV(tag, args "", ##__VA_ARGS__); \
            }                                          \
        }                                              \
    } while (0)

#define LOG_BUFFER_HEX(tag, buffer, buff_len)          \
    do                                                 \
    {                                                  \
        if (LOG_LVL >= ESP_LOG_DEBUG)                  \
        {                                              \
            esp_log_buffer_hex(tag, buffer, buff_len); \
        }                                              \
    } while (0)

#endif // BSP_LOG