#include <stdio.h>
#include "server.h"
#include "client.h"
#include "wifi_handler.h"
#include "printer_helper.h"

void app_main(void)
{
    PRINTFC_MAIN("Main is starting");
    wifi_handler_start();
    server_start();
    client_start();
    PRINTFC_MAIN("Main is done");
}