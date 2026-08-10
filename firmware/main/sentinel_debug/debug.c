#include "debug.h"
#include "esp_pm.h"
#include "esp_core_dump.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_console.h"  
#include "linenoise/linenoise.h"
#include "driver/uart.h"   
#include "esp_sleep.h" 

#include "abstractions/abstractions.h"
#include "nvs_store/nvs_store.h"
#include "nvs.h"
#include "nvs_flash.h"


static const char* TAG = "CORE_DUMP";
static const char* TAGW = "ESP_WAKE";
static const char* TAGR = "ESP_RESET";
//static const char *TAGB = "BOOT";


void check_panic_data(esp_core_dump_summary_t *sum) {
    void* sp = esp_cpu_get_sp();
    
    if(sum == NULL) return;

    if(sum != NULL) {
        //if theres a summary then that'll mean core dump availabvle
        //now this is when I check if it was able to get the summary
        if(esp_core_dump_get_summary(sum) == ESP_OK) {
            if(xSemaphoreTake(printMutex, portMAX_DELAY)) {
                ESP_LOGD(TAG, "--- Found a crash report from the previous session! ---");
                ESP_LOGD(TAG, "Died in task: %s", sum->exc_task);
                ESP_LOGD(TAG, "Exception Cause: %d", sum->ex_info.exc_cause);
                ESP_LOGD(TAG, "Exception PC: 0x%" PRIx32, sum->exc_pc);

                ESP_LOGD(TAG, "Fault Address EXCVADDR: 0x%" PRIx32, sum->ex_info.exc_vaddr);
                ESP_LOGD(TAG, "SP: %p", sp);
                ESP_LOGD(TAG, "Return Addr LR (A0): 0x08%" PRIx32, sum->ex_info.exc_a);
                
            } 

            xSemaphoreGive(printMutex);
            
        } 

        else ESP_LOGI(TAG, "Clean Sys Boot. No crash data detected.");
        
    }

    
}

void esp_wake_reason(void) {

    printf("\n--- WAKE REASON ---\n");

    uint32_t cause = esp_sleep_get_wakeup_causes();

    if(cause == 0) {
        mutex_log('I', TAGW, "Esp wake by Std Power-On/RST");
        return; 
    }
    if (cause & BIT(ESP_SLEEP_WAKEUP_TIMER)) {
        mutex_log('I', TAGW, "Esp wake by Wakeup Timer");
    }
    
    if (cause & BIT(ESP_SLEEP_WAKEUP_EXT0)) {
        mutex_log('I', TAGW, "Esp wake by GPIO or btn.");
    }

    uint32_t masks = BIT(ESP_SLEEP_WAKEUP_TIMER) | BIT(ESP_SLEEP_WAKEUP_EXT0);
    if (cause & ~masks) {
        mutex_log('I', TAGW, "Other raw active wake reasons mask: 0x%" PRIx32, cause);
    }
}

void esp_rst_reason(void) {
    esp_reset_reason_t rst = esp_reset_reason();

    printf("\n--- RESET REASON ---\n");

    switch(rst) {
        case ESP_RST_POWERON:
            mutex_log('I', TAG, "Esp reset by Manual Cold boot! Normal reboot.");
        break;
        case ESP_RST_WDT:
            mutex_log('I', TAG, "Esp hard reset by General Watchdog.");
        break;
        case ESP_RST_TASK_WDT:
            mutex_log('I', TAG, "Esp hard reset by Task Watchdog.");
        break;
        case ESP_RST_SW:
            mutex_log('I', TAG, "Esp reset by calling esp_restart(). Hint: Could be OTA");
        break;

        default:
            mutex_log('I', TAG, "Reset reason: %d.", rst);
    }
}
void activate_safe_mode() {

    mutex_log('I', "SAFE_MODE", "--- SENTINEL SAFE MODE: MAIN MENU ---");

    esp_wake_reason();

    esp_rst_reason();
}

