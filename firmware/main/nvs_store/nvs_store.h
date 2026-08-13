#pragma once

#include "nvs_flash.h"
#include "esp_err.h"
#include <stdint.h>

bool isBootLoop(void);
esp_err_t init_nvs(void);

void namespace_open(const char* name);
void close_nvs(void);
void str_nvs_set(const char* key, const char *val);
void str_nvs_get(const char* key, char *val, size_t max_size);

esp_err_t nvs_reset(bool cmd);
int32_t nvs_increment_cb(const char* key);