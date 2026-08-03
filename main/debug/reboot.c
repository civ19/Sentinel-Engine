#include "esp_pm.h"
#include "esp_core_dump.h"
#include "esp_log.h"
#include "esp_cpu.h"

#include "abstractions/abstractions.h"
#include "reboot.h"

static const char* TAG = "CORE_DUMP";
static const char *TAGB = "BOOT";

void check_panic_data() {
    void* sp = esp_cpu_get_sp();
    esp_core_dump_summary_t *sum = malloc(sizeof(esp_core_dump_summary_t));

    if(sum != NULL) {
        //if theres a summary => core dump availabvle
        //now we check if it was able to get the summary
        if(esp_core_dump_get_summary(sum) == ESP_OK) {
            mutex_log('D', TAG, "--- Found a crash report from the previous session! ---");
            mutex_log('D', TAG, "Died in task: %s", sum->exc_task);
            mutex_log('D', TAG, "Exception Cause: %d", sum->ex_info.exc_cause);
            mutex_log('D', TAG, "EPC Line: 0x%" PRIx32, sum->exc_pc);

            mutex_log('D', TAG, "Fault Address EXCVADDR: 0x%" PRIx32, sum->ex_info.exc_vaddr);
            mutex_log('D', TAG, "SP: %p", sp);
            mutex_log('D', TAG, "Return Addr LR (A0): 0x%"  PRIx32, sum->ex_info.exc_a);

            
            mutex_log('I', TAG, "Core dump flash partition NOTTT erased successfully.");
        } else mutex_log('I', TAG, "Clean Sys Boot. No crash data detected.");
        
        
    }

    free(sum);
}

