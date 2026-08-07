#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "freertos/semphr.h"

#include  "abstractions/abstractions.h"
#include "sentinel_debug/debug.h"
#include "wdt/wdt.h"
#include "ota/ota.h"
#include "tasks/ota_task.h"

void trigger_null_ptr_crash() {

    ESP_LOGI("SYS", "About to crash now...");
    vTaskDelay(pdMS_TO_TICKS(500));
    volatile int* bad_ptr = NULL;
    *bad_ptr = 42;
}

void app_main(void) {
    printMutex = xSemaphoreCreateMutex();

    ESP_LOGI("SYS", "About to show core dump...");
    check_panic_data();
    
    //xTaskCreatePinnedToCore(wdt_test_task, "wdtTest", 4096, NULL, 2, &test_handle, 0);
    xTaskCreatePinnedToCore(perform_ota_task, "OtaTask", 4096, NULL, 2, NULL, 1);


    
}


