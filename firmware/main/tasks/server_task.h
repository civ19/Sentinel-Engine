#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t server_ip_handle;
void server_prov_task(void *pv);
void trigger_server_prov(const char* dyn_server_ip);