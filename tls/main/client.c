#include "client.h"
#include "printer_helper.h"

static void client_task(void *p)
{
    client_init_param_t *param = (client_init_param_t *)p;

    PRINTFC_CLIENT("client is starting");

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

void client_start(client_init_param_t *param)
{
    xTaskCreate(client_task, "client task", 8192, param, 1, NULL);
}