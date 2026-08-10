#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_event.h"
#include "freertos/event_groups.h"


#include  "abstractions/abstractions.h"
#include "sentinel_debug/debug.h"
#include "wdt/wdt.h"
#include "ota/ota.h"
#include "tasks/ota_task.h"
#include "nvs_store/nvs_store.h"
#include "tasks/cmd_task.h"
#include "tasks/w_task.h"
#include "tasks/server_task.h"
#include "sentinel_debug/cmd.h"
#include "wifi/wifi.h"
#include "ble_prov/prov_master.h"

static const char *TAG = "MAIN";
void trigger_null_ptr_crash() {

    ESP_LOGI("SYS", "About to crash now...");
    vTaskDelay(pdMS_TO_TICKS(500));
    volatile int* bad_ptr = NULL;
    *bad_ptr = 42;
}

void loop_validation_task(void *pv) {
    mutex_log('I', TAG, "Verifying Boot Integrity...");
    vTaskDelay(pdMS_TO_TICKS(2000)); //10s check. the esp gets 2s of good execution to prove its not on a boot loop
    
    mutex_log('I', TAG, "Device stabilized successfully. Clearing boot tracking metrics.");
    nvs_reset(0);

    vTaskDelete(NULL);

}


void app_main(void) {

    app_evt_group = xEventGroupCreate();
    printMutex = xSemaphoreCreateMutex();

    
    //check if we have no boot loops first. secuity checks
    if((init_nvs() != ESP_OK)) esp_restart();

    if(isBootLoop()) {
        activate_safe_mode();
        return;
    }

    xTaskCreatePinnedToCore(loop_validation_task, "LoopValid", 4096, NULL, 1, NULL, 0);
    esp_err_t ret = esp_ota_mark_app_valid_cancel_rollback(); 
    if(ret != ESP_OK)  mutex_log('E', TAG, "Failed to cancel rollback (Normal if running from factory slot). rc=%d", ret);

    xTaskCreatePinnedToCore(wifi_connect_task, "wifiConnect", 4096, NULL, 3, &wifi_task_handle, 1);
    xTaskCreatePinnedToCore(server_prov_task, "serverConnect", 4096, NULL, 3, &server_ip_handle, 1);
    if(ble_prov_task() != ESP_OK) {
        mutex_log('E', TAG, "Failed to init NimBLE Provisioning Stack.");
        return;
    }

    //gatekeeper
    mutex_log('I', TAG, "BLE active. Waiting for wifi BT provisioning...");
    xEventGroupWaitBits(app_evt_group, WIFI_CONN_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    mutex_log('I', TAG, "Wifi set. Waiting for MQTT BT provisioning...");
    xEventGroupWaitBits(app_evt_group, SVR_CONN_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    mutex_log('I', TAG, "We are online!");

    xTaskCreatePinnedToCore(perform_ota_task, "OtaUpdateTask", 8192, NULL, 2, NULL, 1); //finally runnign ota


 
}


