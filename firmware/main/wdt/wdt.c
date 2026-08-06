#include "wdt.h"
#include "esp_task_wdt.h"
#include "freertos/task.h"

TaskHandle_t test_handle = NULL;
TaskHandle_t ota_h = NULL;

void wdt_init(void) {
    esp_task_wdt_config_t wdt_conf = {
        .idle_core_mask = (1 << 0),//cpu core bitmask - cpu 0
        .timeout_ms = 1000,
        .trigger_panic = true,
    };

    ESP_ERROR_CHECK(esp_task_wdt_reconfigure(&wdt_conf));
    if(test_handle != NULL) ESP_ERROR_CHECK(esp_task_wdt_add(test_handle));
    if(ota_handle != NULL) 

}


void wdt_test_task(void *pv) {
    while(1); //hung deliberately
}