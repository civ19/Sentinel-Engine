#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t wifi_task_handle;
void wifi_connect_task(void *pv);
void trigger_wifi_provisioning(const char* ssid, const char* pass);