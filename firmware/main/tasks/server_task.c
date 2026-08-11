#include "server_task.h"
#include "ble_prov/nimble_gatt.h"
#include "ota/ota.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#include "abstractions/abstractions.h"
#include "wifi/wifi.h"

static char dyn_svr_ip[MAX_SVR_LEN + 1];

TaskHandle_t server_ip_handle = NULL;

void server_prov_task(void *pv) {
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(500));

        mutex_log('I', "SVR", "Server IP Received.");
        xEventGroupSetBits(app_evt_group, SVR_CONN_BIT);
    }

}

const char* get_ip(void) {return dyn_svr_ip;}

void trigger_server_prov(const char* dyn_svr_ip) {
    mutex_log('I', "SVR", "Trigger received IP: %s", dyn_svr_ip);

    xEventGroupClearBits(app_evt_group, SVR_CONN_BIT); //clearing the bit before i copy it and then setting the bit after it copies successfully
    strlcpy((char*) dyn_svr_ip, dyn_svr_ip,  sizeof(dyn_svr_ip));

    if(server_ip_handle != NULL) xTaskNotifyGive(server_ip_handle);

}

