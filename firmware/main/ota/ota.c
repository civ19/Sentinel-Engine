#include "ota.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "cJSON.h"

#include "abstractions/abstractions.h"

#define RESP_BUF 512

static char resp_buf[RESP_BUF];
static int resp_len = 0;
static const char* TAG = "OTA";
static const char* TAGS = "OTA Server";

esp_http_client_handle_t client_handle;

void manage_endpoints() {
    mutex_log('I', TAG, "Attempting to sync data over HTTPS...");

    esp_err_t initial_resp = esp_http_client_perform(client_handle); //acts as a courier. goes to our spring server, endpoints and all

    if(initial_resp == ESP_OK) {
        //if it connected to the server - wifi layer
        mutex_log('I', TAGS, "ESP Connected to server!");
        
        int status_rc = esp_http_client_get_status_code(client_handle);
        if(status_rc == 200) { 
            mutex_log('I', TAGS, "Data Endpoint Fetch Successful! 200 OK.");
        }
        else mutex_log('E', TAGS, "Server REJECTED data! Status code: %d", status_rc);
    } else mutex_log('I', TAGS, "Server handshake failed. %d", esp_err_to_name(initial_resp));


}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            manage_endpoints(); 
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

    client_handle = esp_http_client_init(&http_conf);

    if(client_handle == NULL) {
        mutex_log('E', TAG, "Failed to allocate esp_http_client memory handle.");
        return;
    }

}


