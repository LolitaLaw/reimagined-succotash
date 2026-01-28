#include "bsp_wifi.h"

// 事件重试计数器
static int s_retry_num = 0;
#define EXAMPLE_ESP_MAXIMUM_RETRY 5

// WiFi 事件回调处理函数
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    wifi_event_ap_staconnected_t *event = NULL;
    wifi_event_ap_stadisconnected_t *event_dis = NULL;
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            // 处理STA模式下的设备连接和断开事件
        case WIFI_EVENT_STA_START: // WIFI启动
            esp_wifi_connect();
            ESP_LOGI("", "WiFi STA启动，连接AP…");
            break;
        case WIFI_EVENT_STA_DISCONNECTED: // WIFI断开连接
            ESP_LOGI("", "WiFi STA断开连接，正在重新连接...");

            if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
            {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGW("", "连接路由器失败，正在重试 (%d/%d)...", s_retry_num, EXAMPLE_ESP_MAXIMUM_RETRY);
            }
            else
                ESP_LOGE("", "连接路由器失败，停止重试。AP 模式依然可用。");
            break;
            // 处理AP模式下的设备连接和断开事件
        case WIFI_EVENT_AP_STACONNECTED:
            event = (wifi_event_ap_staconnected_t *)event_data;
            ESP_LOGI("", "新设备连入 AP " MACSTR ", AID=%d", MAC2STR(event->mac), event->aid);
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            event_dis = (wifi_event_ap_stadisconnected_t *)event_data;
            ESP_LOGI("", "设备断开 AP " MACSTR ", AID=%d", MAC2STR(event_dis->mac), event_dis->aid);
            break;
        default:
            break;
        }
    }
    // 处理STA模式下获取IP地址事件(代表联网成功)
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI("", "WiFi STA获取IP地址：" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0; // 重试计数器清零
    }
}

void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());                            // 初始化网络接口(TCP/IP协议栈)
    ESP_ERROR_CHECK(esp_event_loop_create_default());             // 事件循环
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta(); // 创建默认STA网络接口
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();   // 创建默认AP网络接口
    // (可选) 这里的指针 sta_netif/ap_netif 可以保存下来供后续修改 IP 等使用
    (void)sta_netif;
    (void)ap_netif;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); // 初始化 默认WiFi配置
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));                // 初始化WiFi驱动
    // 注册WiFi和IP事件的事件处理函数
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));
    // 配置STA模式参数
    wifi_config_t sta_cfg = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    // 配置AP模式参数
    wifi_config_t ap_cfg = {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .password = WIFI_AP_PASSWORD,
            .channel = WIFI_AP_CHANNEL,
            .max_connection = WIFI_AP_MAX_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    if (strlen(WIFI_AP_PASSWORD) == 0)
        ap_cfg.ap.authmode = WIFI_AUTH_OPEN;
    // 设置模式为 STA+AP
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    // 设置WiFi配置
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    // 启动WiFi
    ESP_LOGI("", "Starting WiFi...");
    ESP_ERROR_CHECK(esp_wifi_start());
}