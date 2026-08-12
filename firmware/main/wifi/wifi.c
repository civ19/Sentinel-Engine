#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_wifi.h" 
#include "esp_netif.h" 
#include "freertos/event_groups.h"
#include <assert.h>

#include "abstractions/abstractions.h"
#include "wifi.h"

static const char *TAG = "EVENT_LOOP";

EventGroupHandle_t app_evt_group = NULL;

static uint8_t retry_ctr = 0;
static uint8_t max_retry = 5;

void reconnect() {
    if(retry_ctr < max_retry) {
        esp_wifi_connect();
        retry_ctr++;
        
        if(xSemaphoreTake(printMutex, portMAX_DELAY)){
            ESP_LOGI(TAG, "Retry to connect to AP: (%d/%d)", retry_ctr, max_retry);
            xSemaphoreGive(printMutex);
        }
    }
    else {
        if(xSemaphoreTake(printMutex, portMAX_DELAY)){
            ESP_LOGE(TAG, "Failed to connect after maximum retries.");
            xSemaphoreGive(printMutex);
        }
    }
}

static void wifi_event_callback(void *arg, esp_event_base_t dept, int32_t event_id, void* data) { //manages wifi state
    if(dept == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if(xSemaphoreTake(printMutex, portMAX_DELAY)) {
            ESP_LOGW(TAG, "Disconnected. Reconnecting...");
            xSemaphoreGive(printMutex);
            
        }
        reconnect(); //reconnect asap
        xEventGroupClearBits(app_evt_group, WIFI_CONN_BIT); //not ready. clearing bit 0
       
    }

    else if(dept == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        retry_ctr = 0;
        ip_event_got_ip_t *event_data = (ip_event_got_ip_t* )data;
        xEventGroupSetBits(app_evt_group, WIFI_CONN_BIT);
        if(xSemaphoreTake(printMutex, portMAX_DELAY)) { 
            ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event_data->ip_info.ip));
            xSemaphoreGive(printMutex);
        }
    }

}

void reg_wifi_events() { 
    
    ESP_ERROR_CHECK(esp_event_loop_create_default()); 
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_callback, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_event_callback, NULL, NULL));

}

void init_wifi_hardware() {

    ESP_ERROR_CHECK(esp_netif_init());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_conf = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_conf));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); 

}

void wifi_conf(const char *dyn_ssid, const char *dyn_pass) {
    assert(dyn_ssid != NULL);
    assert(dyn_pass != NULL);

    wifi_config_t wifi_conf = {};
    strlcpy((char*)wifi_conf.sta.ssid, dyn_ssid, sizeof(wifi_conf.sta.ssid));
    strlcpy((char*)wifi_conf.sta.password, dyn_pass, sizeof(wifi_conf.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_conf));

    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_ERROR_CHECK(esp_wifi_connect());

}