#include "nvs_store.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"
#include <assert.h>

#include "abstractions/abstractions.h"
#include "safe_mode/debug.h"


static const char* TAG = "NVS";
static nvs_handle_t nvs_h;


esp_err_t nvs_reset(bool cmd) {
    //cmd meaning if it came from the reset crash AND boot cmd. if not, were just gonna reset the boot only not the crash t
    assert(nvs_h != 0);
    
    esp_err_t ret;

    ret = nvs_set_i32(nvs_h, "boot_count", 0);
    if(ret != ESP_OK) mutex_log('E',TAG, "NVS write failed for Boot Count. Error: %s", esp_err_to_name(ret));

    if(cmd == true) { //as in, if its not the cmd but from just resetting the boot only 
        ret = nvs_set_i32(nvs_h, "crash_count", 0);
        if(ret != ESP_OK) mutex_log('E',TAG, "NVS write failed for Crash Count. Error: %s", esp_err_to_name(ret));
    }

    ret = nvs_commit(nvs_h);
    if (ret != ESP_OK) {
        mutex_log('E', TAG, "NVS commit failed: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t init_nvs(void) { //initializing nvs 

    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if (ret != ESP_OK) if (ret != ESP_OK) {
        mutex_log('E', TAG, "NVS initialization failed.");
        return ret;
    }

    namespace_open("Debug");

    assert(nvs_h != 0);

    return ret;

}

void str_nvs_set(const char* key, const char *val) { //2 helpers to make main and safe_cmd use it as a black box. so we dont have to extern our handle just to set or get from nvs
    if(nvs_h == 0) init_nvs();

    assert(nvs_h != 0);
    assert(key != NULL);
    assert(val != NULL);

    esp_err_t ret = nvs_set_str(nvs_h, key, val);
    if(ret != ESP_OK) mutex_log('E',TAG, "NVS String Set failed. Error: %s", esp_err_to_name(ret));

    nvs_commit(nvs_h);

}

esp_err_t str_nvs_get(const char* key, char *out_buf, size_t max_size) {
    if(nvs_h == 0) init_nvs();

    assert(nvs_h != 0);
    assert(key != NULL);
    assert(out_buf != NULL);

    size_t size = max_size;

    esp_err_t ret = nvs_get_str(nvs_h, key, out_buf, &size);
    if(ret != ESP_OK) {
        mutex_log('E',TAG, "NVS String Set failed. Error: %s", esp_err_to_name(ret));
        return ret;
    }

    return ret;

}


void namespace_open(const char* name) {
    esp_err_t ret = nvs_open(name, NVS_READWRITE, &nvs_h);
    if (ret != ESP_OK) {
        mutex_log('E', TAG, "NVS namespace open failed.");
    }

    assert(nvs_h != 0);
}

bool isBootLoop(void) {
    //if cnt > 3 its boot looped 
    //fuirst get anbd set the boot counter. if its >3 then we return true. else return false

    int32_t n = nvs_increment_cb("boot_count");
    if(n>=3) return true;
    else return false;
    
}

void close_nvs(void) {
    nvs_close(nvs_h);  //for main to use after were done with nvs
}


int32_t nvs_increment_cb(const char *key) {

    assert(nvs_h != 0);
    assert(key != NULL);

    int32_t cnt = 0;

    //reading curr val:
    esp_err_t ret = nvs_get_i32(nvs_h, key, &cnt);
    if(ret == ESP_ERR_NVS_NOT_FOUND) cnt = 1;
    else if(ret != ESP_OK) {
        mutex_log('E',TAG, "NVS read failed for key: %s. Error: %s", key, esp_err_to_name(ret));
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


