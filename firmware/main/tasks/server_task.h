#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SVR_CONN_BIT (1 << 1)

extern TaskHandle_t server_ip_handle;
extern char server_ip[];

void server_prov_task(void *pv);
void trigger_server_prov(const char* dyn_svr_ip);


const char* get_ip(void);