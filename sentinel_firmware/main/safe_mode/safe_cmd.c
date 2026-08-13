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
#include "wifi/wifi.h"

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

BaseType_t force_ota_help(void) {

    static char saved_ssid[MAX_SSID_LEN];
    static char saved_pass[MAX_PASS_LEN];
    static char saved_ip[MAX_SVR_LEN];

    memset(saved_ssid, 0, sizeof(saved_ssid));
    memset(saved_pass, 0, sizeof(saved_pass));
    memset(saved_ip, 0, sizeof(saved_ip));

    //getting the creds
    if (str_nvs_get("wifi_ssid", saved_ssid, sizeof(saved_ssid)) != ESP_OK || //massive if for checking all 3
        str_nvs_get("wifi_pass", saved_pass, sizeof(saved_pass)) != ESP_OK ||
        str_nvs_get("server_ip", saved_ip, sizeof(saved_ip)) != ESP_OK) {

        return pdFAIL;
    }

    reg_wifi_events();
    init_wifi_hardware();

    wifi_conf(saved_ssid, saved_pass); //connecting to wifi

    
    printf("Waiting for WiFi connection...\n");
    EventBits_t bits = xEventGroupWaitBits(app_evt_group, WIFI_CONN_BIT, pdFALSE, pdTRUE, pdMS_TO_TICKS(10000));

    if (!(bits & WIFI_CONN_BIT)) {
        printf("Error: Could not connect to WiFi. Check credentials in NVS.\n");
        return pdFAIL;
    }

    printf("WiFi Connected! Syncing time for HTTPS...\n"); //syncing time again with ip
    sync_time();

   
    BaseType_t task_ret = xTaskCreatePinnedToCore(perform_ota_task, "OtaForce", 8192, (void *)saved_ip, 2, NULL, 1);
    return task_ret;
    
}
static int do_force_ota(int argc, char** argv) {
    if(argc > 1) {
        printf("Error: Invalid syntax. Simply type 'ota_force' with no extra arguments.\n");
        return 1;
    }

    if(isOtaRunning == false) {
        isOtaRunning = true;

        if(force_ota_help() != pdPASS) {
            isOtaRunning = false;
            return 1;
       }
    } 
    else {
        printf("Error: A firmware download task is already actively processing blocks.\n");
        return 1;
    } 

    printf("\nUser issued forced OTA Update. Downloading firmware update...\n");

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
    printf("\nDont forget to CLEAR!\n\n");

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

    printf("Cleared successfully.\n");

    return 0;
}