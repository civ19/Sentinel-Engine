#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "freertos/semphr.h"
#include "esp_ota_ops.h"

#include  "abstractions/abstractions.h"
#include "sentinel_debug/debug.h"
#include "wdt/wdt.h"
#include "ota/ota.h"
#include "tasks/ota_task.h"
#include "nvs_store/nvs_store.h"

const char *TAG = "MAIN";
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
    
    //check if we have no boot loops first. secuity checks
    if((init_nvs() != ESP_OK)) esp_restart();

    xTaskCreatePinnedToCore(loop_validation_task, "LoopValid", 4096, NULL, 1, NULL, 0);

    esp_err_t ret = esp_ota_mark_app_valid_cancel_rollback(); 

    if(isBootLoop) {
        activate_safe_mode();
        return;
    }
    
    //main app tasks

    
}


