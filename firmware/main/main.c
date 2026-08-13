#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_event.h"
#include "freertos/event_groups.h"


#include  "abstractions/abstractions.h"
#include "safe_mode/debug.h"
#include "wdt/wdt.h"
#include "ota/ota.h"
#include "tasks/ota_task.h"
#include "nvs_store/nvs_store.h"
#include "tasks/w_task.h"
#include "tasks/server_task.h"
#include "safe_mode/safe_cmd.h"
#include "wifi/wifi.h"
#include "ble_prov/prov_master.h"
#include "esp_sntp.h"

static const char *TAG = "MAIN";

void loop_validation_task(void *pv) {
    mutex_log('I', TAG, "Verifying Boot Integrity...");
    vTaskDelay(pdMS_TO_TICKS(2000)); //10s check. the esp gets 2s of good execution to prove its not on a boot loop
    
    mutex_log('I', TAG, "Device stabilized successfully. Clearing boot tracking metrics.");
    nvs_reset(0);

    vTaskDelete(NULL);

}


void sync_time() {
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();
    
    int retry = 0;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < 10) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void app_main(void) {

    
    reg_wifi_events();
    //check if we have no boot loops first. secuity checks
    app_evt_group = xEventGroupCreate();
    printMutex = xSemaphoreCreateMutex();

    if((init_nvs() != ESP_OK)) esp_restart();    

    if(isBootLoop()) {
        activate_safe_mode();
        return;
    } 

    init_wifi_hardware();
    sync_time();


    xTaskCreatePinnedToCore(loop_validation_task, "LoopValid", 4096, NULL, 1, NULL, 0);
    esp_err_t ret = esp_ota_mark_app_valid_cancel_rollback(); 
    if(ret != ESP_OK)  mutex_log('E', TAG, "Failed to cancel rollback (Normal if running from factory slot). rc=%d", ret);

    namespace_open("Credentials"); //for storing credentiuals
    xTaskCreatePinnedToCore(wifi_connect_task, "wifiConnect", 4096, NULL, 5, &wifi_task_handle, 1);
    xTaskCreatePinnedToCore(server_prov_task, "serverConnect", 4096, NULL, 5, &server_ip_handle, 1);
    if(ble_prov_task() != ESP_OK) {
        mutex_log('E', TAG, "Failed to init NimBLE Provisioning Stack.");
        return;
    }

    //gatekeeper
    mutex_log('I', TAG, "BLE active. Waiting for Wifi BT provisioning...");
    xEventGroupWaitBits(app_evt_group, WIFI_CONN_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    mutex_log('I', TAG, "Wifi set. Waiting for Server BT provisioning...");
    xEventGroupWaitBits(app_evt_group, SVR_CONN_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    mutex_log('I', TAG, "We are online!");

    close_nvs();

    xTaskCreatePinnedToCore(perform_ota_task, "OtaUpdateTask", 8192, NULL, 2, NULL, 1); //finally runnign ota


 
}


