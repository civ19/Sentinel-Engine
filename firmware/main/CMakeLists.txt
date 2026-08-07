idf_component_register(
    SRCS "main.c"
         "sentinel_debug/debug.c"
         "wdt/wdt.c"
         "abstractions/abstractions.c"
         "ota/ota.c"
         "wifi/wifi.c"
         "tasks/ota_task.c"
         "tasks/w_task.c"
         "nvs_store/nvs_store.c"

    INCLUDE_DIRS "."

    REQUIRES 
        esp_pm
        esp_timer
        app_update
        esp_system
        espcoredump
        freertos
        cjson
        esp_http_client
        esp_https_ota
        esp_wifi
        esp_netif
        esp_event
        nvs_flash
        console


    EMBED_TXTFILES
        "sentinel_cert.pem"
)