#pragma once

#include "nvs_flash.h"
#include <stdint.h>

void nvs_init(void);
nvs_handle_t open_namespace(const char* name);
int32_t nvs_increment_cb(nvs_handle_t nvs_h, const char* key, void (*on_error_cb)(void));