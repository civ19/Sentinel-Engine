#pragma once

#include "nvs_flash.h"
#include "esp_err.h"
#include <stdint.h>

void nvs_init(void);
void nvs_clear(nvs_handle_t nvs_h);

extern nvs_handle_t nvs_handle;
nvs_handle_t open_namespace(const char* name);
int32_t nvs_increment_cb(nvs_handle_t nvs_h, const char* key, void (*on_error_cb)(void));