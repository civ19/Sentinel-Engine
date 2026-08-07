#include "esp_pm.h"
#include "esp_core_dump.h"
#include "esp_log.h"
#include "esp_cpu.h"

#include "abstractions/abstractions.h"
#include "debug.h"

static const char* TAG = "CORE_DUMP";
static const char *TAGB = "BOOT";


void check_panic_data() {
    void* sp = esp_cpu_get_sp();
    esp_core_dump_summary_t *sum = malloc(sizeof(esp_core_dump_summary_t));

    if(sum != NULL) {
        //if theres a summary => core dump availabvle
        //now we check if it was able to get the summary
        if(esp_core_dump_get_summary(sum) == ESP_OK) {
            if(xSemaphoreTake(printMutex, portMAX_DELAY)) {
                ESP_LOGD(TAG, "--- Found a crash report from the previous session! ---");
                ESP_LOGD(TAG, "Died in task: %s", sum->exc_task);
                ESP_LOGD(TAG, "Exception Cause: %d", sum->ex_info.exc_cause);
                ESP_LOGD(TAG, "Exception PC: 0x%" PRIx32, sum->exc_pc);

                ESP_LOGD(TAG, "Fault Address EXCVADDR: 0x%" PRIx32, sum->ex_info.exc_vaddr);
                ESP_LOGD(TAG, "SP: %p", sp);
                ESP_LOGD(TAG, "Return Addr LR (A0): 0x08%" PRIx32, sum->ex_info.exc_a);
                
                ESP_ERROR_CHECK(esp_core_dump_image_erase());
                ESP_LOGI(TAG, "Core dump flash partition erased successfully.");
            } 
            else ESP_LOGI(TAG, "Clean Sys Boot. No crash data detected.");
            
        } 

        xSemaphoreGive(printMutex);
        
        
    }

    free(sum);
}

