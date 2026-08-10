#include "prov_master.h"
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "nimble/nimble_port_freertos.h"


#include "nimble_gatt.h"
#include "gap_evt.h"
#include "abstractions/abstractions.h"

static const char* TAG = "BLE_TASK";

void nimble_port_task(void* param);

esp_err_t ble_prov_task(void) {

    int rc = nimble_port_init();
    if(rc!=0) {
        mutex_log('E', TAG, "Init failed! Memory alloc arror: rc=%d", rc);
        return ESP_FAIL;
    }

    ble_hs_cfg.sync_cb = ble_sync_radio; //syncing the radio 

    ble_svc_gap_init();
    ble_svc_gatt_init();

    //profile structure init and validation
    rc = ble_gatts_count_cfg(gatt_svr_svcs); //allocating the right memory for tghe gatt table
    if(rc!=0) {
        mutex_log('E', TAG, "GATT Table Memory alloc arror: rc=%d", rc);
        return ESP_FAIL;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if(rc!=0) {
        mutex_log('E', TAG, "Failed to add GATT table to BT Database pool: rc=%d", rc);
        return ESP_FAIL;
    }

    //name for dev
    rc = ble_svc_gap_device_name_set("ESP32_Node");
    if(rc!=0) {
        mutex_log('E', TAG, "Failed to apply public gap node name: rc=%d", rc);
        return ESP_FAIL;
    }

    //task on core 
    mutex_log('I', TAG, "Starting async BT task on core %d...", xPortGetCoreID());
    nimble_port_freertos_init(nimble_port_task);

    return ESP_OK;


}
