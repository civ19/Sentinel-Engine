#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "freertos/semphr.h"
#include "esp_ota_ops.h"
#include "esp_console.h"  
#include "linenoise/linenoise.h"
#include "driver/uart.h" 

#include  "abstractions/abstractions.h"
#include "sentinel_debug/debug.h"
#include "wdt/wdt.h"
#include "ota/ota.h"
#include "tasks/ota_task.h"
#include "nvs_store/nvs_store.h"
#include "tasks/cmd_task.h"
#include "tasks/w_task.h"
#include "sentinel_debug/cmd.h"

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

void init_console() {
    esp_console_config_t cons_conf = ESP_CONSOLE_CONFIG_DEFAULT();
    cons_conf.max_cmdline_args = 8;
    esp_console_init(&cons_conf);

    esp_cmd_conf();

}
void app_main(void) {


    
    //check if we have no boot loops first. secuity checks
    if((init_nvs() != ESP_OK)) esp_restart();

    xTaskCreatePinnedToCore(loop_validation_task, "LoopValid", 4096, NULL, 1, NULL, 0);

    esp_err_t ret = esp_ota_mark_app_valid_cancel_rollback(); 
    if(ret != ESP_OK)  mutex_log('E', TAG, "Failed to cancel rollback (Normal if running from factory slot). rc=%d", ret);

    if(isBootLoop()) {
        activate_safe_mode();
        return;
    }
    
    //main app tasks

    init_console();

    xTaskCreatePinnedToCore(diagnostic_console_task, "ConsoleTask", 4096, NULL, 5, NULL, 1);





    
}


