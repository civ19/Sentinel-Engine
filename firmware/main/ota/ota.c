#include "ota.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "cJSON.h"

#include "abstractions/abstractions.h"

static const char* TAG = "OTA";

void manage_endpoints(void) {
    
}
static esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            
        break;

        default:
        break;
    }

    return ESP_OK;
}
void init_http_client(void) {

    esp_http_client_config_t http_conf = {
        .url = "https://172.17.35.33", //add nimBLE prov on this later
        .cert_pem = (const char *)server_cert_pem_start,
        .event_handler = _http_event_handler,
        
    };

    esp_http_client_handle_t http_handle = esp_http_client_init(&http_conf);

    if(http_handle == NULL) {
        mutex_log('E', TAG, "Failed to allocate esp_http_client memory handle.");
        return;
    }

    

}


static const char* TAG = "OTA";
bool wifi_conn = true;


void pre_update(const char* url) {
    //write, verify and flash pre reboot
    const char* ota_url = url;
    const char* server_cert_pem = "-----BEGIN CERTIFICATE-----\n...\n-----END CERTIFICATE-----";

    if(ota_url == NULL || server_cert_pem == NULL) {
        ESP_LOGE(TAG, "Url or Certification null. Please provide it and try again.");
        return;
    }

    esp_http_client_config_t http_conf = {};
    http_conf.url = ota_url;
    http_conf.cert_pem = (char *)server_cert_pem;
    
    esp_https_ota_config_t ota_conf = {
        .http_config = &http_conf,
    };

    esp_err_t rc = esp_https_ota(&ota_conf);
    if(rc == ESP_OK) {
        ESP_LOGI(TAG, "Update complete! Rebooting...");
        esp_restart();
    } else ESP_LOGE(TAG, "OTA failed. Staying on current firmware.");
}

void validate_new_firmware(void) { //post update - check if the flashed data is actually safe, then conbfirm

    ESP_LOGI(TAG, "Starting post-boot health checks...");
    vTaskDelay(pdMS_TO_TICKS(2000)); //2s wait

    if(wifi_conn) {
        esp_err_t ret = esp_ota_mark_app_valid_cancel_rollback(); //green flag to not rollback 
        if(ret == ESP_OK) ESP_LOGI("OTA", "App validated! Rollback disabled - New ver is PERMANENT.");
        else ESP_LOGE("OTA", "Health check failed. May rollback on next boot.");

    }
}

void perform_ota_update(const char* url) {
    pre_update(url); //to signify we have a new update
    validate_new_firmware(); //validates if its safe

}


its compiling the entire .pio folder, so... just review this in the meantime

