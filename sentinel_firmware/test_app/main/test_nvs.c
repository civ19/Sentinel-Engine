#include "unity.h"
#include "nvs_store/nvs_store.h"
#include "nvs_flash.h"
#include <inttypes.h>
#include "esp_log.h"

void setUp(void) { //starting from a clean slate. kinda like @BeforeEach then cleaning the db in junit integratrion testing
    esp_err_t ret;

    ret = nvs_flash_deinit();
    TEST_ASSERT_EQUAL_INT32(ESP_ERR_NVS_NOT_INITIALIZED, ret);
    
    ret = nvs_flash_erase();


    ret = init_nvs();
    TEST_ASSERT_EQUAL_INT32(ESP_OK, ret);
    
    
}

void tearDown(void) 
{
    nvs_flash_deinit();
}


TEST_CASE("Boot loop detector returns TRUE after 3 sequential reboots, so we can run Safe mode", "[SAFE_MODE_TRIGGER]")
{
    
    for(int i = 0; i<3; i++) nvs_increment_cb("boot_count");
    
    TEST_ASSERT_TRUE(isBootLoop());
}

TEST_CASE("Boot loop detector returns FALSE for LESS than 3 sequential reboots, so we can run Safe mode", "[SAFE_MODE_TRIGGER]")
{
    
    nvs_increment_cb("boot_count"); //this will only be like 2. so this fits the condition

    TEST_ASSERT_FALSE(isBootLoop());
}

TEST_CASE("NVS Increment CB increments it by 1 everytime its called.", "[NVS]")
{
    
    int32_t mock_n1 = nvs_increment_cb("boot_count");
    int32_t mock_n2 = nvs_increment_cb("boot_count");
    
   
    TEST_ASSERT_EQUAL(1, mock_n1);
    TEST_ASSERT_EQUAL(2, mock_n2);

}


void force_link_nvs(void) {}