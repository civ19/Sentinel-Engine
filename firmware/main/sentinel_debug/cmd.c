#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_console.h"  
#include "linenoise/linenoise.h"
#include "driver/uart.h"    

#include "abstractions/abstractions.h"
#include "nvs_store/nvs_store.h"

static const char* TAG = "CMD";

static int do_clear_nvs(int argc, char **argv) {
    //argc = num of words typed and argv is the array contianing the strings 

    if(argc > 1) {
        printf("Error: Invalid syntax. Simply type 'clear' with no extra arguments.\n");
        return -1;
    }

    mutex_log('I', TAG, "User issues manual clear override. Resetting nvs crash counter...");
    nvs_clear();

    
}

