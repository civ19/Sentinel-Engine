#include "unity.h"
#include "nvs_store/nvs_store.h"
#include "nvs_flash.h"

void reset(void) { //starting from a clean slate. kinda like @BeforeEach then cleaning the db in junit integratrion testing
    esp_err_t ret;

    ret = nvs_flash_deinit();
    TEST_ASSERT_EQUAL_INT32(ESP_OK, ret);
    ret = nvs_flash_erase();
    TEST_ASSERT_EQUAL_INT32(ESP_OK, ret);

    ret = init_nvs();
    TEST_ASSERT_EQUAL_INT32(ESP_OK, ret);
    
    
}

TEST_CASE("Boot loop detector actually returns after 3 sequential reboots, so we can run Safe mode", ["NVS"])
{
    nvs_increment_cb("boot_cnt");
}
