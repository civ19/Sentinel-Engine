#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "abstractions/abstractions.h"
#include "esp_wifi.h"
#include "string.h"


#include "w_task.h"
#include "wifi/wifi.h"


TaskHandle_t wifi_task_handle = NULL;

static char dyn_ssid[33];
static char dyn_pass[85];

void wifi_connect_task(void *pv) {
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); 
        
        vTaskDelay(pdMS_TO_TICKS(500)); 

        mutex_log('I', "WIFI_TASK", "Credentials received. Stopping BLE and starting WiFi...");
        
        wifi_conf(dyn_ssid, dyn_pass);
    }
}

void trigger_wifi_provisioning(const char* ssid, const char* pass) {
    assert(ssid != NULL);
    
    strlcpy(dyn_ssid, ssid, sizeof(dyn_ssid));
    strlcpy(dyn_pass, pass, sizeof(dyn_pass));

    if (wifi_task_handle != NULL) {
        xTaskNotifyGive(wifi_task_handle);
    }
}