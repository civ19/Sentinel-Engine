idf_component_register(
    SRCS "main.c"
         "sentinel_debug/debug.c"
         "sentinel_debug/safe_cmd.c"
         "wdt/wdt.c"
         "abstractions/abstractions.c"
         "ota/ota.c"
         "wifi/wifi.c"
         "tasks/ota_task.c"
         "tasks/w_task.c"
         "tasks/server_task.c"
         "nvs_store/nvs_store.c"
         "ble_prov/nimble_gatt.c"
         "ble_prov/gap_evt.c"
         "ble_prov/prov_master.c"
         

    INCLUDE_DIRS "."

    REQUIRES 
        esp_pm
        esp_timer
        app_update
        esp_system
        espcoredump
        freertos
        esp_http_client
        esp_https_ota
        esp_wifi
        esp_netif
        esp_event
        nvs_flash
        console
        driver
        bt


    EMBED_TXTFILES
        "sentinel_cert.pem"
)