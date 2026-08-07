#include "ota_task.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string.h>
#include <stdio.h>

#include "abstractions/abstractions.h"
#include "ota/ota.h"

void perform_ota_task(void *pv) {
    mutex_log('I', TAG, "Starting OTA Update Task...");
    
    int status_code = get_status_code();
    char *hash_header = get_hash_header();

    esp_https_ota_handle_t ota_handle = NULL;
    
    init_ota(&ota_handle);

    updated_check(ota_handle, status_code);

    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    if(strlen(hash_header) > 0) mutex_log('I', TAGS, "Server provided SHA256 checksum: %s", hash_header);

    mutex_log('I', TAG, "Downloading new firmware binary");

    for(;;) { //streaming data chunks for downloading and flashing
        esp_err_t ret = esp_https_ota_perform(ota_handle);
        esp_task_wdt_reset(); //feeding the dog
        if(ret != ESP_ERR_HTTPS_OTA_IN_PROGRESS) break;
    }

    if(esp_https_ota_is_complete_data_received(ota_handle)) {
        esp_err_t ret = esp_https_ota_finish(ota_handle);
        if(ret == ESP_OK) {
            mutex_log('I', TAG, "Firmware Update Complete! Rebooting...");
            esp_restart();
        }
        else mutex_log('E', TAG, "OTA Finish Failed. Return code: 0x%x", ret);
    } else {
        mutex_log('E', TAG, "OTA Data Stream Failed or connection timed out.");
        esp_https_ota_abort(ota_handle);
    }

    esp_task_wdt_delete(NULL);
    vTaskDelete(NULL);


}

