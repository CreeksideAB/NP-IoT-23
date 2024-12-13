#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#ifndef SERVER_H
#define SERVER_H

typedef struct server_init_param_t
{
    EventGroupHandle_t wifi_event_group;
    // int wifi_event_group,
} server_init_param_t;

void server_start(server_init_param_t param);

#endif