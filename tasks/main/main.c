#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/stream_buffer.h"
#include "freertos/message_buffer.h"

#define TASK1_TEXT "123456789"
#define TASK2_TEXT "abcdefghi"

#define BIT1 (1 << 0)
#define BIT2 (1 << 1)
#define BIT3 (1 << 2)

typedef struct function_data_t
{
    SemaphoreHandle_t semaphore;
    SemaphoreHandle_t countingSemaphore;
    SemaphoreHandle_t mutex;
    EventGroupHandle_t event_group;

    QueueHandle_t queue;
    MessageBufferHandle_t messageBuffer;
    StreamBufferHandle_t streamBuffer;

    char sharedString[10];
} function_data_t;

const char *red = "\033[0;31m";
const char *green = "\033[0;32m";
const char *yellow = "\033[0;33m";
const char *blue = "\033[0;34m";
const char *magenta = "\033[0;35m";
const char *cyan = "\033[0;36m";

const char *reset = "\033[0m";

#define PRINTFL(format, ...) printf("%s"          \
                                    ":"           \
                                    "%d " format, \
                                    __FILE__, __LINE__, ##__VA_ARGS__)

#define PRINTFC(color, format, ...) printf("%s" format "%s", color, ##__VA_ARGS__, reset)

#define PRINTFC_MAIN(format, ...) \
    PRINTFC(red, "main: ");       \
    printf(format, ##__VA_ARGS__)

#define PRINTFC_FUNCTION_1(format, ...) \
    PRINTFC(blue, "Function 1: ");      \
    printf(format, ##__VA_ARGS__)

#define PRINTFC_FUNCTION_2(format, ...) \
    PRINTFC(yellow, "Function 2: ");    \
    printf(format, ##__VA_ARGS__)

#define PRINTFC_FUNCTION_3(format, ...) \
    PRINTFC(magenta, "Function 3: ");   \
    printf(format, ##__VA_ARGS__)

TaskHandle_t task_handle_1;
TaskHandle_t task_handle_2;
TaskHandle_t task_handle_3;
function_data_t functionData;

void readSharedString(char *string, char *shared_string)
{
    for (size_t i = 0; i < 9; i++)
    {
        string[i] = shared_string[i];
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void writeSharedString(char *string, char *shared_string)
{
    for (size_t i = 0; i < 9; i++)
    {
        shared_string[i] = string[i];
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void function_1(void *parameters)
{

    function_data_t *param = (function_data_t *)parameters;

    char string[10] = {0};

    xSemaphoreTake(param->mutex, portMAX_DELAY);
    PRINTFC_FUNCTION_1("Function 1 reading string\n");
    readSharedString(string, param->sharedString);
    PRINTFC_FUNCTION_1("Function 1 read %s\n", string);
    xSemaphoreGive(param->mutex);

    PRINTFC_FUNCTION_1("Nu väntar jag på notification\n");
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    PRINTFC_FUNCTION_1("Nu fick jag på notification\n");

    PRINTFC_FUNCTION_1("Nu väntar jag på counting\n");
    BaseType_t result = xSemaphoreTake(param->countingSemaphore, portMAX_DELAY);
    if (result == pdTRUE)
    {
        PRINTFC_FUNCTION_1("Jag har counting\n");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    PRINTFC_FUNCTION_1("Nu släpper jag counting\n");
    xSemaphoreGive(param->countingSemaphore);

    vTaskDelay(pdMS_TO_TICKS(1000));

    PRINTFC_FUNCTION_1("Nu släpper jag semaphore\n");
    xSemaphoreGive(param->semaphore);

    PRINTFC_FUNCTION_1("Nu sätter vi event group bit 1\n");
    xEventGroupSetBits(param->event_group, BIT1);
    xEventGroupWaitBits(param->event_group, BIT1 | BIT2 | BIT3, pdFALSE, pdTRUE, portMAX_DELAY);

    PRINTFC_FUNCTION_1("Nu stänger vi av\n");
    vTaskDelete(NULL);
}

void function_2(void *parameters)
{
    function_data_t *param = (function_data_t *)parameters;

    vTaskDelay(pdMS_TO_TICKS(300));
    char *string = TASK2_TEXT;
    xSemaphoreTake(param->mutex, portMAX_DELAY);
    writeSharedString(string, param->sharedString);
    xSemaphoreGive(param->mutex);

    PRINTFC_FUNCTION_2("Nu väntar jag på counting\n");
    BaseType_t result = xSemaphoreTake(param->countingSemaphore, portMAX_DELAY);
    if (result == pdTRUE)
    {
        PRINTFC_FUNCTION_2("Jag har counting\n");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    PRINTFC_FUNCTION_2("Nu släpper jag counting\n");
    xSemaphoreGive(param->countingSemaphore);

    PRINTFC_FUNCTION_2("Nu väntar jag på semaphore\n");
    result = xSemaphoreTake(param->semaphore, portMAX_DELAY);
    if (result == pdTRUE)
    {
        // lyckats
        PRINTFC_FUNCTION_2("Jag har nu tagigt semaphore.\n");
    }
    else
    {
        // inte lyckats
        PRINTFC_FUNCTION_2("Det gick fel\n");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    PRINTFC_FUNCTION_2("Nu släpper jag semaphore\n");
    xSemaphoreGive(param->semaphore);

    PRINTFC_FUNCTION_2("Nu sätter vi event group bit 2\n");
    xEventGroupSetBits(param->event_group, BIT2);
    xEventGroupWaitBits(param->event_group, BIT1 | BIT2 | BIT3, pdFALSE, pdTRUE, portMAX_DELAY);

    int data_received;
    char text[3];
    xQueueReceive(param->queue, &data_received, portMAX_DELAY);
    PRINTFC_FUNCTION_2("Nu stänger vi av %d \n", data_received);
    xStreamBufferReceive(param->streamBuffer, text, 3, portMAX_DELAY);
    xMessageBufferReceive(param->messageBuffer, text, 3, portMAX_DELAY);
    vTaskDelete(NULL);
}

void function_3(void *parameters)
{
    function_data_t *param = (function_data_t *)parameters;

    vTaskDelay(pdMS_TO_TICKS(300));
    char *string = TASK2_TEXT;
    xSemaphoreTake(param->mutex, portMAX_DELAY);
    writeSharedString(string, param->sharedString);
    xSemaphoreGive(param->mutex);

    PRINTFC_FUNCTION_3("Nu väntar jag på counting\n");
    BaseType_t result = xSemaphoreTake(param->countingSemaphore, portMAX_DELAY);
    if (result == pdTRUE)
    {
        PRINTFC_FUNCTION_3("Jag har counting\n");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    PRINTFC_FUNCTION_3("Nu släpper jag counting\n");
    xSemaphoreGive(param->countingSemaphore);

    PRINTFC_FUNCTION_3("Nu väntar jag på semaphore\n");
    result = xSemaphoreTake(param->semaphore, portMAX_DELAY);
    if (result == pdTRUE)
    {
        // lyckats
        PRINTFC_FUNCTION_3("Jag har nu tagigt semaphore.\n");
    }
    else
    {
        // inte lyckats
        PRINTFC_FUNCTION_3("Det gick fel\n");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    PRINTFC_FUNCTION_3("Nu släpper jag semaphore\n");
    xSemaphoreGive(param->semaphore);

    PRINTFC_FUNCTION_3("Nu sätter vi event group bit 3\n");
    xEventGroupSetBits(param->event_group, BIT3);
    xEventGroupWaitBits(param->event_group, BIT1 | BIT2 | BIT3, pdFALSE, pdTRUE, portMAX_DELAY);

    int data_int = 5;
    xQueueSend(param->queue, &data_int, 0);

    char text[8] = "1234567";
    xStreamBufferSend(param->streamBuffer, text, 8, portMAX_DELAY);
    xMessageBufferSend(param->messageBuffer, text, 8, portMAX_DELAY);

    PRINTFC_FUNCTION_3("Nu stänger vi av\n");
    vTaskDelete(NULL);
}

void app_main(void)
{
    functionData.semaphore = xSemaphoreCreateBinary();
    functionData.countingSemaphore = xSemaphoreCreateCounting(2, 2);
    for (size_t i = 0; i < 9; i++)
    {
        functionData.sharedString[i] = TASK1_TEXT[i];
    }
    functionData.mutex = xSemaphoreCreateMutex();
    functionData.event_group = xEventGroupCreate();
    functionData.queue = xQueueCreate(10, sizeof(int));
    functionData.messageBuffer = xMessageBufferCreate(10);
    functionData.streamBuffer = xStreamBufferCreate(10, 3);

    PRINTFC_MAIN("Starting task 1\n");
    xTaskCreate(function_1, "Task 1", 2048, &functionData, 1, &task_handle_1);
    PRINTFC_MAIN("Starting task 2\n");
    xTaskCreate(function_2, "Task 2", 2048, &functionData, 1, &task_handle_2);
    PRINTFC_MAIN("Starting task 3\n");
    xTaskCreate(function_3, "Task 3", 2048, &functionData, 1, &task_handle_3);

    vTaskDelay(pdMS_TO_TICKS(100));

    PRINTFC_MAIN("Notifying Task 1\n");
    xTaskNotifyGive(task_handle_1);
}