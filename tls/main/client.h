#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#ifndef CLIENT_H
#define CLIENT_H

typedef struct client_init_param_t
{
    EventGroupHandle_t wifi_event_group;
    // int wifi_event_group,
} client_init_param_t;

void client_start(client_init_param_t *param);

#endif