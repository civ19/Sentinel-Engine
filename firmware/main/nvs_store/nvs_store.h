#pragma once

#include "nvs_flash.h"
#include "esp_err.h"
#include <stdint.h>

esp_err_t nvs_init(void);
esp_err_t nvs_reset(void);
int32_t nvs_increment_cb(const char* key, void (*on_error_cb)(void));