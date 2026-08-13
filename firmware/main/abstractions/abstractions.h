#pragma once

#include "freertos/FreeRTOS.h" 
#include "freertos/semphr.h"
#include "esp_err.h"

extern SemaphoreHandle_t printMutex;

void mutex_log(char type, const char *tag, const char *format, ...);

#define CHECK_ERR(expr, action) do { \
    esp_err_t _ret = (expr); \
    if (_ret != ESP_OK) { \
        mutex_log('E', TAG, "Error 0x%X (%s) at %s:%d", _ret, esp_err_to_name(_ret), __FILE__, __LINE__); \
        action; \
    } \
} while(0)