#include "ota_task.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <string.h>
#include <stdio.h>

#include "abstractions/abstractions.h"
#include "ota/ota.h"
#include "sentinel_debug/cmd.h"
#include "tasks/server_task.h"
#include "wifi/wifi.h"

const char *TAG = "OTA";
const char *TAGS = "OTA Server";

static bool upd_success = true; //cjhecking if it successfully updated. if not, the bit got the svr gets cleared

void perform_ota_task(void *pv) {
    mutex_log('I', TAG, "Starting OTA Update Task...");
    
    int status_code = get_status_code();
    char *hash_header = get_hash_header();

    esp_https_ota_handle_t ota_handle = NULL;
    
    init_ota(&ota_handle, get_ip());


    updated_check(ota_handle, status_code);

    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    if(strlen(hash_header) > 0) mutex_log('I', TAGS, "Server provided SHA256 checksum: %s", hash_header);

    mutex_log('I', TAG, "Downloading new firmware binary");

    esp_err_t ret;
    for(;;) { //streaming data chunks for downloading and flashing
        ret = esp_https_ota_perform(ota_handle);
        esp_task_wdt_reset(); //feeding the dog
        if(ret != ESP_ERR_HTTPS_OTA_IN_PROGRESS) break;
    }

    xEventGroupSetBits(app_evt_group, SVR_CONN_BIT);

    if(esp_https_ota_is_complete_data_received(ota_handle)) {
        esp_err_t ret = esp_https_ota_finish(ota_handle);
        if(ret == ESP_OK) {
            mutex_log('I', TAG, "Firmware Update Complete! Rebooting...");
            esp_restart();
        } else {
            mutex_log('E', TAG, "OTA Finish Failed. Return code: 0x%x", ret);
            upd_success = false;
        }
    } else {
        mutex_log('E', TAG, "OTA Data Stream Failed or connection timed out.");
        upd_success = false;
        esp_https_ota_abort(ota_handle);
    }

    if(upd_success == false) xEventGroupClearBits(app_evt_group, SVR_CONN_BIT); 
    set_ota_bool(false);
    esp_task_wdt_delete(NULL);
    vTaskDelete(NULL);


}

