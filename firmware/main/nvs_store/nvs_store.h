#pragma once

#include "nvs_flash.h"
#include "esp_err.h"
#include <stdint.h>

bool isBootLoop(void);
esp_err_t init_nvs(void);

void namespace_open(const char* name);
void close_nvs(void);
void str_nvs(const char* key, const char *val);

esp_err_t nvs_reset(bool cmd);
int32_t nvs_increment_cb(const char* key);