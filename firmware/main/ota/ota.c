#include "ota.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include <string.h>
#include <stdio.h>

#include "abstractions/abstractions.h"

#define CURR_VER "1.0.0"
#define SERVER_IP "172.17.35.33" //will provision this later via nimble


static const char* TAG = "OTA";
static const char* TAGS = "OTA Server";

esp_http_client_handle_t client_handle;


esp_err_t validate_img_header(esp_app_desc_t *new_app_info) {
    if(new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_app_desc_t *app_desc = esp_app_get_description();
    mutex_log('I', TAG, "Current app version: &s", app_desc->version);
    mutex_log('I', TAG, "Updated(new) app version: %s", new_app_info->version);


    return ESP_OK; //for anti rollback
}

void init_http_client(void) {

    esp_http_client_config_t http_conf = {
        .url = "https://172.17.35.33", //add nimBLE prov on this later
        .cert_pem = (const char *)server_cert_pem_start,
        
    };

    client_handle = esp_http_client_init(&http_conf);

    if(client_handle == NULL) {
        mutex_log('E', TAG, "Failed to allocate esp_http_client memory handle.");
        return;
    }

}







