#include "wifi_handler.h"
#include "printer_helper.h"
#include "esp_system.h"

void wifi_handler_start()
{
    PRINTFC_WIFI_HANDLER("WiFi Handler is starting");

    PRINTFC_WIFI_HANDLER("Using ssid: %s%s%s", green, CONFIG_WIFI_SSID, reset);
    PRINTFC_WIFI_HANDLER("Using password: %s%s%s", green, CONFIG_WIFI_PASSWORD, reset);
}