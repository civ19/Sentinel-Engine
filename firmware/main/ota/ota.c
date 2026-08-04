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


