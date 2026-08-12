#include "unity.h"
#include "nvs_store/nvs_store.h"
#include "nvs_flash.h"

void setup(void) { //starting from a clean slate. kinda like @BeforeEach then cleaning the db in junit integratrion testing
    esp_err_t ret;

    ret = nvs_flash_deinit();
    TEST_ASSERT_EQUAL_INT32(ESP_OK, ret);
    ret = nvs_flash_erase();
    TEST_ASSERT_EQUAL_INT32(ESP_OK, ret);

    ret = init_nvs();
    TEST_ASSERT_EQUAL_INT32(ESP_OK, ret);
    
    
}

TEST_CASE("Boot loop detector returns TRUE after 3 sequential reboots, so we can run Safe mode", "[SAFE_MODE_TRIGGER]")
{
    for(int i = 0; i<3; i++) nvs_increment_cb("boot_count");

    TEST_ASSERT_TRUE(isBootLoop());
}

TEST_CASE("Boot loop detector returns FALSE for LESS than 3 sequential reboots, so we can run Safe mode", "[SAFE_MODE_TRIGGER]")
{
    nvs_increment_cb("boot+count"); //this will only be like 2. so this fits the condition

    TEST_ASSERT_FALSE(isBootLoop());
}

