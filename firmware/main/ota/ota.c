#include "ota.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"

#include <string.h>
#include <stdio.h>

#include "abstractions/abstractions.h"


#define SERVER_IP "172.17.35.33" //will provision this later via nimble
#define OTA_URL_SIZE 256

static const char* TAG = "OTA";
static const char* TAGS = "OTA Server";

static int status_code = 0;
static char global_hash_header[65] = {0}; //sha256 hash buf


void updated_check(esp_https_ota_handle_t handle, int status) {
    if(status == 304) {
        mutex_log('I', TAGS, "Backend returned 304: Firmware is already up to date.");
        esp_https_ota_abort(handle);
        vTaskDelete(NULL);

    }
}

esp_err_t validate_img_header(esp_app_desc_t *new_app_info) {
    if(new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_app_desc_t *app_desc = esp_app_get_description();
    mutex_log('I', TAG, "Current app version: &s", app_desc->version);
    mutex_log('I', TAG, "Updated(new) app version: %s", new_app_info->version);


    return ESP_OK; //for anti rollback
}


esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ON_HEADER:
            if(strcasecmp(evt->header_key, "X-Sentinel-Hash") == 0) {
                snprintf(global_hash_header, sizeof(global_hash_header), "%s",evt->header_value);
            }
        break;

        case HTTP_EVENT_ON_FINISH:
            status_code = esp_http_client_get_status_code(evt->client);
        break;

        default:
        break;
    }

    return ESP_OK;
}

esp_https_ota_handle_t init_ota(void) {

    const esp_app_desc_t* app_desc = esp_app_get_description(); //app desc
    char url[OTA_URL_SIZE];
    snprintf(url, sizeof(url), "https://%s:8080/api/ota/check?ver=%s", SERVER_IP, app_desc->version);


    //http init
    esp_http_client_config_t http_conf = {
        .url = url, //adding  nimBLE prov on this later
        .cert_pem = (const char *)server_cert_pem_start,
        .skip_cert_common_name_check = true, //skipping cn
        .keep_alive_enable = true,
        .timeout_ms = 10000,
        .event_handler = _http_event_handler,
    };

    esp_https_ota_config_t ota_conf = { //ota conf
        .http_config = &http_conf,
    };

    esp_https_ota_handle_t update_handle = NULL;
    esp_err_t ret = esp_https_ota_begin(&ota_conf, &update_handle);
    if(ret != ESP_OK) {
        mutex_log('E', TAG, "ESP HTTPS OTA Begin failed (Check network connection or cert data)");
        vTaskDelete(NULL);
    }

    return update_handle;

}

void perform_ota_task(void *pv) {
    mutex_log('I', TAG, "Starting OTA Update Task...");
    
    status_code = 0;
    memset(global_hash_header, 0, sizeof(global_hash_header));

    esp_ota_handle_t ota_handle = init_ota();

    updated_check(ota_handle, status_code);

    if(strlen(global_hash_header) > 0) mutex_log('I', TAGS, "Server provided SHA256 checksum: %s", global_hash_header);

    for(;;) { //streaming data chunks for downloading and flashing
        esp_err_t ret = esp_https_ota_perform(ota_handle);
        if(ret != ESP_ERR_HTTPS_OTA_IN_PROGRESS) break;
    }

    if(esp_https_ota_is_complete_data_received) {
        esp_err_t ret = esp_https_ota_finish(ota_handle);
        if(ret == ESP_OK) {
            mutex_log('I', TAG, "Firmware Update Complete! Rebotting...");
            esp_restart();
        }
        else mutex_log('E', TAG, "OTA Finish Failed. Return code: 0x%x", ret);
    } else {
        mutex_log('E', TAG, "OTA Data Stream Failed or connection timed out.");
        esp_https_ota_abort(ota_handle);
    }

    vTaskDelete(NULL);


}







