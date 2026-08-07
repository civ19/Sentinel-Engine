#include "nvs_store.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"

#include "abstractions/abstractions.h"
#include "sentinel_debug/debug.h"


static const char* TAG = "NVS";
static nvs_handle_t nvs_h;


esp_err_t nvs_reset(void) {
    esp_err_t ret = nvs_set_i32(nvs_h, "boot_count", 0);
    if(ret != ESP_OK) mutex_log('E',TAG, "NVS write failed for Boot Count. Error: %s", esp_err_to_name(ret));

    ret = nvs_set_i32(nvs_h, "crash_count", 0);
    if(ret != ESP_OK) mutex_log('E',TAG, "NVS write failed for Crash Count. Error: %s", esp_err_to_name(ret));

    ret = nvs_commit(nvs_h);
    if (ret != ESP_OK) {
        mutex_log('E', TAG, "NVS commit failed: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t nvs_init(void) { //initializing nvs 
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if (ret != ESP_OK) if (ret != ESP_OK) {
        mutex_log('E', TAG, "NVS initialization failed.");
        return ret;
    }

    ret = nvs_open("Debug", NVS_READWRITE, &nvs_h);
    if (ret != ESP_OK) {
        mutex_log('E', TAG, "NVS namespace open failed.");
    }

    return ret;

}


int32_t nvs_increment_cb(const char *key, void (*on_error_cb)(void)) {
    int32_t cnt = 0;

    //reading curr val:
    esp_err_t ret = nvs_get_i32(nvs_h, key, &cnt);
    if(ret == ESP_ERR_NOT_FOUND) cnt = 1;
    else if(ret != ESP_OK) {
        mutex_log('E',TAG, "NVS read failed for key: %s. Error: %s", key, esp_err_to_name(ret));

        if(on_error_cb != NULL) {
            on_error_cb();
        }
        return -1;
    }
    else {
        cnt++; //if things go well and theres no read errors, we can just increment 
    }

    ret = nvs_set_i32(nvs_h, key, cnt);
    if(ret != ESP_OK) mutex_log('E',TAG, "NVS write failed for key: %s. Error: %s", key, esp_err_to_name(ret));

    nvs_commit(nvs_h);

    return cnt;
}


