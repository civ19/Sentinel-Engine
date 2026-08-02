#include "esp_pm.h"
#include "esp_core_dump.h"
#include "esp_log.h"


void check_panic_data() {
    esp_core_dump_summary_t *sum = malloc(sizeof(esp_core_dump_summary_t));

    if(sum != NULL) {
        //if theres a summary => core dump availabvle
        
    }
}