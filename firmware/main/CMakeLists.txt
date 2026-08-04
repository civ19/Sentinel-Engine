idf_component_register(
    SRCS "main.c"
         "debug/reboot.c"
         "wdt/wdt.c"
         "abstractions/abstractions.c"
         "ota/ota.c"
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
    EMBED_TXTFILES
        "sentinel_cert.pem"
)