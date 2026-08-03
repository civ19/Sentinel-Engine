#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_pm.h"

#include  "abstractions/abstractions.h"
#include "debug/reboot.h"

void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(500));
    volatile int *ptr = NULL;
    
    *ptr = 42;
    panic_reboot_check();
}
