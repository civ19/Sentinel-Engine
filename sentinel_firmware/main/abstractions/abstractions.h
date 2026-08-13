#pragma once

#include "freertos/FreeRTOS.h" 
#include "freertos/semphr.h"
#include "esp_err.h"

extern SemaphoreHandle_t printMutex;

void mutex_log(char type, const char *tag, const char *format, ...);

#define CHECK_ERR(expr, action) do { \
    esp_err_t __macro_err = (expr); \
    if (__macro_err != ESP_OK) { \
        mutex_log('E', TAG, "Error 0x%X (%s) at %s:%d", __macro_err, esp_err_to_name(__macro_err), __FILE__, __LINE__); \
        action; \
    } \
} while(0)

