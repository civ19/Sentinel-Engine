idf_component_register(
    SRCS "main.c"
         "debug/reboot.c"
         "wdt/wdt.c"
         "abstractions/abstractions.c"
         "ota/ota.c"
         "wifi/wifi.c"
         "tasks/ota_task.c"
         "pwm/pwm.c"

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


    EMBED_TXTFILES
        "sentinel_cert.pem"
)