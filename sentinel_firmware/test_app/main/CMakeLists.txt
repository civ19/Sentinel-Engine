get_filename_component(ROOT_LOGIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../main" ABSOLUTE)

idf_component_register(
    SRCS "main.c"
         "test_ota.c"
         "test_nvs.c"
         "${ROOT_LOGIC_DIR}/ota/ota.c"
         "${ROOT_LOGIC_DIR}/nvs_store/nvs_store.c"
         "${ROOT_LOGIC_DIR}/abstractions/abstractions.c" # Add the source for abstractions too!
    INCLUDE_DIRS "." 
                 "${ROOT_LOGIC_DIR}"         # <--- THIS IS THE FIX. It lets ota.c see "abstractions/"
                 "${ROOT_LOGIC_DIR}/ota" 
                 "${ROOT_LOGIC_DIR}/nvs_store"
                 "${ROOT_LOGIC_DIR}/abstractions"
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
    
    PRIV_REQUIRES unity
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC UNIT_TEST)
target_link_libraries(${COMPONENT_LIB} PRIVATE "-Wl,--wrap=sentinel_ota_abort")