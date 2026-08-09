#pragma once

#include "nvs_flash.h"
#include "esp_err.h"
#include <stdint.h>

bool isBootLoop(void);
esp_err_t init_nvs(void);
esp_err_t nvs_reset(bool cmd);
int32_t nvs_increment_cb(const char* key, void (*on_error_cb)(void));