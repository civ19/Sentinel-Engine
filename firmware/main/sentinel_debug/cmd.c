#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_console.h"  
#include "linenoise/linenoise.h"
#include "driver/uart.h"    

#include "abstractions/abstractions.h"
#include "nvs_store/nvs_store.h"

static const char* TAG = "CMD";

static int do_force_ota(int argc, char** argv);
static int do_reboot(int argc, char** argv);
static int do_clear_nvs(int argc, char **argv);

void esp_cmd_conf(void) {
    
    const esp_console_cmd_t clear_cmd = {
        .command = "clear",
        .help = "Reset the NVS crash and boot loop counters to 0. Just type clear.",
        .func = &do_clear_nvs,
    };

    const esp_console_cmd_t ota_cmd = {
        .command = "ota_force",
        .help =  "Bypasses safe moden lock to trigger an immediate OTA check if the current firmware is too bugged. Do ota_force.",
        .func = &ota_cmd,
    };

    const esp_console_cmd_t reboot_cmd = {
        .command = "ota_force",
        .help =  "Execute an immediate ESP32-S3 hardware reset.",
        .func = &do_reboot,
    };

}

static int do_clear_nvs(int argc, char **argv) {
    //argc = num of words typed and argv is the array contianing the strings 

    if(argc > 1) {
        printf("Error: Invalid syntax. Simply type 'clear' with no extra arguments.\n");
        return 1;
    }

    mutex_log('I', TAG, "User issues manual clear override. Resetting nvs crash counter...");
    if (nvs_reset() == ESP_OK) {
        printf("NVS variables wiped and committed. Run 'reboot' to attempt normal startup.\n");
        return 0;
    } else {
        printf("Error: Failed to write recovery metrics to storage chip.\n");
        return 1;
    }
    
}



