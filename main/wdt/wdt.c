#include "wdt.h"
#include "esp_task_wdt.h"
#include "freertos/task.h"

TaskHandle_t test_handle = NULL;

void wdt_init(void) {
    esp_task_wdt_config_t wdt_conf = {
        .idle_core_mask = (1 << 0),//cpu core bitmask - cpu 0
        .timeout_ms = 1000,
        .trigger_panic = true,
    };

    ESP_ERROR_CHECK(esp_task_wdt_init(&wdt_conf));
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));


}

