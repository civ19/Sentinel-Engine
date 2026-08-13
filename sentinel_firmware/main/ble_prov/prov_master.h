#pragma once

#include "esp_err.h"
#include "nimble/nimble_port_freertos.h"
#include "nimble/nimble_port.h"

esp_err_t ble_prov_task(void);
void ble_host_task(void* pv);