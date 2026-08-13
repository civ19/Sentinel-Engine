#include "unity.h"
#include "ota.h"
#include "esp_https_ota.h"
#include <assert.h>



TEST_CASE("Updated_Check() actually stops the OTA Update if no latest binary was found.", "[OTA]")
{
    int mock_addr = 0;
    esp_https_ota_handle_t mock_handle = (esp_https_ota_handle_t)&mock_addr;

    esp_err_t ret = updated_check(mock_handle, 304);

    TEST_ASSERT_EQUAL(ESP_OK, ret);
}

void force_link_ota(void) {}
