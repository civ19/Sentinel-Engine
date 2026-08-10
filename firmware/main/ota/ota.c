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

static const char *TAG = "OTA";
static const char *TAGS = "OTA Server";

static int status_code = 0;
static char global_hash_header[65] = {0}; //sha256 hash buf

int get_status_code() {return status_code;} //getters. i'll be using these in ota_task.c
char* get_hash_header() {return global_hash_header;}

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

    const esp_app_desc_t *app_desc = esp_app_get_description();
    mutex_log('I', TAG, "Current app version: %s", app_desc->version);
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

esp_err_t init_ota(esp_https_ota_handle_t *out_handle) { //double ptr 

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

    esp_err_t ret = esp_https_ota_begin(&ota_conf, out_handle);
    if(ret != ESP_OK) {
        mutex_log('E', TAG, "ESP HTTPS OTA Begin failed (Check network connection or cert data)");
        vTaskDelete(NULL);
    }

    return ret;

}








