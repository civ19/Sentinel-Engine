#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t test_handle;
TaskHandle_t ota_handle;

void wdt_init();
void wdt_test_task(void* pv);