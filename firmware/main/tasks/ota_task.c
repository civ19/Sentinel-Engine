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
#include "safe_mode/safe_cmd.h"
#include "wdt/wdt.h"
#include "ble_prov/nimble_gatt.h"


const char *TAG = "OTA";
const char *TAGS = "OTA Server";

static bool upd_success = true; //cjhecking if it successfully updated. if not, the bit got the svr gets cleared

void perform_ota_task(void *pv) {

    assert(server_ip != NULL);

    wdt_ota_conf();
    esp_https_ota_handle_t ota_handle = NULL;

    mutex_log('I', TAG, "DEBUG IN TASK - Value of passed IP: '%s'", server_ip);

    esp_err_t b_ret = init_ota(&ota_handle, server_ip);
    if(b_ret != ESP_OK) {
        set_ota_bool(false);
        vTaskDelete(NULL);
        return;
    }

    mutex_log('I', TAG, "Starting OTA Update Task...");
    
    int status_code = esp_https_ota_get_status_code(ota_handle);
    if(updated_check(ota_handle, status_code) != ESP_OK || status_code == 304) {
        mutex_log('I', TAG, "Clean exit: App is fully up to date.");
        set_ota_bool(false);
        vTaskDelete(NULL); // Abort handled inside updated_check, terminate safely
        return;
    }

    char *hash_header = get_hash_header();
    assert(hash_header != NULL);


    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    if(strlen(hash_header) > 0) mutex_log('I', TAGS, "Server provided SHA256 checksum: %s", hash_header);
    

    mutex_log('I', TAG, "Downloading new firmware binary");

    esp_err_t ret;
    bool validated = false;
    
    for(;;) { //streaming data chunks for downloading and flashing
        ret = esp_https_ota_perform(ota_handle);

        if(!validated) {
            esp_app_desc_t new_app_info;
            if(esp_https_ota_get_img_desc(ota_handle, &new_app_info) == ESP_OK) {
                if(validate_img_header(&new_app_info) != ESP_OK) {
                    mutex_log('E', TAG, "Image validation failed! Aborting...");
                    esp_https_ota_abort(ota_handle);
                    break;
                } 

                mutex_log('I', TAG, "Header validated. Proceeding with download...");
                validated = true;
            }
        }
        
        esp_task_wdt_reset(); //feeding the dog
        if(ret != ESP_ERR_HTTPS_OTA_IN_PROGRESS) break;
    }


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

    
    set_ota_bool(false);
    esp_task_wdt_delete(NULL);
    vTaskDelete(NULL);


}

