#include "safe_cmd.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_console.h"  
#include "linenoise/linenoise.h"
#include "driver/uart.h"    
#include "freertos/FreeRTOS.h"
#include "esp_core_dump.h"
#include <stdio.h>

#include "abstractions/abstractions.h"
#include "nvs_store/nvs_store.h"
#include "tasks/ota_task.h"
#include "debug.h"
#include "nvs.h"
#include "ble_prov/nimble_gatt.h"


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

    esp_console_cmd_register(&clear_cmd);


    const esp_console_cmd_t ota_cmd = {
        .command = "ota_force",
        .help =  "Bypasses safe moden lock to trigger an immediate OTA check if the current firmware is too bugged. Do ota_force.",
        .func = &do_force_ota,
    };

    esp_console_cmd_register(&ota_cmd);

    const esp_console_cmd_t reboot_cmd = {
        .command = "reboot",
        .help =  "Execute an immediate ESP32-S3 hardware reset.",
        .func = &do_reboot,
    };

    esp_console_cmd_register(&reboot_cmd);

    const esp_console_cmd_t crash_cmd = {
        .command = "crash_report",
        .help =  "Shows the Core Dump Summary of the previous session. Type crash_report.",
        .func = &do_core_dump,
    };

    esp_console_cmd_register(&crash_cmd);

    const esp_console_cmd_t clear_dump_cmd = {
        .command = "crash_clear",
        .help =  "Erase the Core Dump image. Type crash_clear.",
        .func = &do_clear_dump,
    };

    esp_console_cmd_register(&clear_dump_cmd);

}

static int do_clear_nvs(int argc, char **argv) {
    //argc = num of words typed and argv is the array contianing the strings 

    if(argc > 1) {
        printf("Error: Invalid syntax. Simply type 'clear' with no extra arguments.\n");
        return 1;
    }
    
    mutex_log('I', TAG, "User issued manual clear override. Resetting nvs crash counter...");
    if (nvs_reset(1) == ESP_OK) {
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

    printf("User issued manual reboot. Rebooting ESP32...");
    esp_restart(); 
    
    return 0;

}

void force_ota_help(void) {
    esp_err_t ret;
    //getting the creds
    ret = str_nvs_get("wifi_ssid", wifi_ssid, sizeof(wifi_ssid));
    ret= str_nvs_get("wifi_pass", wifi_ssid, sizeof(wifi_pass));
    ret = str_nvs_get("server_ip", wifi_ssid, sizeof(server_ip));


    
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

    esp_core_dump_summary_t *sum = malloc(sizeof(esp_core_dump_summary_t));

    if (sum == NULL) {
        printf("Error: Microcontroller heap exhausted. Cannot allocate summary structure space.\n");
        return 1;
    }

    printf("User requested core dump. Executing Core Dump Summary...\n");
    check_panic_data(sum);
    printf("\nDont forget to CLEAR!");

    free(sum);
    return 0;

}

static int do_clear_dump(int argc, char** argv) {
    if(argc > 1) {
        printf("Error: Invalid syntax. Simply type 'crash_clear' with no extra arguments.\n");
        return 1;
    }

    printf("User requested clearing the core dump. Clearing the image...");


    esp_err_t err = esp_core_dump_image_erase();

    if (err == ESP_OK) {
        printf("Cleared successfully.\n");
    } else {
        printf("Error: Failed to erase the core dump flash partition (0x%x).\n", err);
    }

    printf("Cleared successfully.");

    return 0;
}