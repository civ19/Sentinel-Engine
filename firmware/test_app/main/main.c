#include "unity.h"
#include "unity_test_runner.h"
#include "freertos/FreeRTOS.h"

void force_link_nvs(void);
void force_link_ota(void);

void app_main(void) {

    force_link_nvs();
    force_link_ota();


    unity_run_menu();
    
    
}