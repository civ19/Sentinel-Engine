#include "esp_event.h"
#pragma once

#define WIFI_CONN_BIT (1 << 0)

extern EventGroupHandle_t wifi_event_group;

void init_nvs_event();
void init_wifi_hardware();
void wifi_conf(const char *dyn_ssid, const char *dyn_pass);



