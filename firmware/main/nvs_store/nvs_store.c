#include "nvs_store.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"

#include "abstractions/abstractions.h"


void nvs_init(void) { //initializing nvs 
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
}

nvs_handle_t open_namespace(const char* name) { //making the debug namespace
    nvs_handle_t nvs_h;
    ESP_ERROR_CHECK(nvs_open("Debug", NVS_READWRITE, &nvs_h));

    return nvs_h;
}

int32_t nvs_boot_track(nvs_handle_t nvs_h) {
    int32_t boot = 0;

    //read boot first. make a value in vs, store it.

    esp_err_t ret = nvs_get_i32(nvs_h, "boot_cnt", &boot);
    if(ret == ESP_ERR_NVS_NOT_FOUND) boot = 1;
    else { //if the value is there already
        ESP_ERROR_CHECK(ret);
        boot++;
    }

    ESP_ERROR_CHECK(nvs_set_i32(nvs_h, "boot_cnt", boot));

    nvs_commit(nvs_h);

    return boot;
}




