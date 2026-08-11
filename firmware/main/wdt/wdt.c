#include "wdt.h"
#include "esp_task_wdt.h"
#include "freertos/task.h"


void wdt_ota_conf(void) {
    esp_task_wdt_config_t wdt_conf = {
        .idle_core_mask = (1 << 1),//cpu core bitmask - cpu 1 because ota task is on core 1
        .timeout_ms = 15000,
        .trigger_panic = true,
    };

    ESP_ERROR_CHECK(esp_task_wdt_reconfigure(&wdt_conf));

}


