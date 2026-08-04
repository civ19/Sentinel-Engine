#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_wifi.h" 
#include "esp_netif.h" 
#include "freertos/event_groups.h"

#include "abstractions/abstractions.h"
#include "wifi.h"

static const char *TAG = "EVENT_LOOP";



