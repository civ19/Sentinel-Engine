#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_console.h"  
#include "linenoise/linenoise.h"
#include "driver/uart.h"    
#include "freertos/FreeRTOS.h"

#include "abstractions/abstractions.h"
#include "nvs_store/nvs_store.h"
#include "tasks/ota_task.h"
#include "debug.h"

static const char* TAG = "CMD";
static bool isOtaRunning =  false;

static int do_force_ota(int argc, char** argv);
static int do_reboot(int argc, char** argv);
static int do_clear_nvs(int argc, char **argv);
static int do_core_dump(int argc, char **argv);
static int do_clear_dump(int argc, char **argv);


void set_ota_bool(bool val) {isOtaRunning = val;}

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
        .command = "reboot",
        .help =  "Execute an immediate ESP32-S3 hardware reset.",
        .func = &do_reboot,
    };

    const esp_console_cmd_t crash_cmd = {
        .command = "crash_report",
        .help =  "Shows the Core Dump Summary of the previous session. Type crash_report.",
        .func = &do_core_dump,
    };

    const esp_console_cmd_t clear_dump_cmd = {
        .command = "crash_clear",
        .help =  "Erase the Core Dump image. Type crash_clear.",
        .func = &do_clear_dump,
    };

}

static int do_clear_nvs(int argc, char **argv) {
    //argc = num of words typed and argv is the array contianing the strings 

    if(argc > 1) {
        printf("Error: Invalid syntax. Simply type 'clear' with no extra arguments.\n");
        return 1;
    }

    mutex_log('I', TAG, "User issued manual clear override. Resetting nvs crash counter...");
    if (nvs_reset() == ESP_OK) {
        printf("NVS variables wiped and committed. Run 'reboot' to attempt normal startup.\n");
        return 0;
    } else {
        printf("Error: Failed to write recovery metrics to storage chip.\n");
        return 1;
    }
    
}

static int do_reboot(int argc, char** argv) {
    if(argc > 1) {
        printf("Error: Invalid syntax. Simply type 'reboot' with no extra arguments.\n");
        return 1;
    }

    mutex_log('I', TAG, "User issued manual reboot. Rebooting ESP32...");
    esp_restart(); //if manual then print esp restarted successfuly : reminder
    
    return 0;

}

static int do_force_ota(int argc, char** argv) {
    if(argc > 1) {
        printf("Error: Invalid syntax. Simply type 'ota_force' with no extra arguments.\n");
        return 1;
    }

    if(isOtaRunning == false) {
        isOtaRunning = true;
        BaseType_t ret = xTaskCreatePinnedToCore(perform_ota_task, "OtaTask", 8192, NULL, 2, NULL, 1);

        if(ret != pdPASS) {
            printf("Error: System failed to allocate necessary heap space for the thread.\n");
            isOtaRunning = false;
            return 1;
        }
    } 
    else {
        printf("Error: A firmware download task is already actively processing blocks.\n");
        return 1;
    } 

    printf("User issued forced OTA Update. Running task...");

    return 0;



}

static int do_core_dump(int argc, char **argv) {
    if(argc > 1) {
        printf("Error: Invalid syntax. Simply type 'crash_report' with no extra arguments.\n");
        return 1;
    }

    printf("User requested core dump. Executing Core Dump Summary...");
    check_panic_data();
    
    return 0;

}