#include <stdio.h>
#include "server.h"
#include "client.h"
#include "wifi_handler.h"
#include "printer_helper.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

EventGroupHandle_t wifi_event_group;
client_init_param_t c_param;
server_init_param_t s_param;
wifi_init_param_t w_param = {
    .ssid = CONFIG_WIFI_SSID,
    .password = CONFIG_WIFI_PASSWORD,
};

void app_main(void)
{
    PRINTFC_MAIN("Main is starting");

    PRINTFC_MAIN("NVS Initialize");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = nvs_flash_init_partition("eol");
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    PRINTFC_MAIN("Creating event group");
    wifi_event_group = xEventGroupCreate();
    w_param.wifi_event_group = wifi_event_group;

    s_param.wifi_event_group = wifi_event_group;
    c_param.wifi_event_group = wifi_event_group;

    PRINTFC_MAIN("Starting all tasks");
    wifi_handler_start(&w_param);
    server_start(&s_param);
    client_start(&c_param);
    PRINTFC_MAIN("Main is done");
}