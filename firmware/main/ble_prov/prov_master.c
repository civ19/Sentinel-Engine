#include "prov_master.h"
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "nimble_gatt.h"
#include "gap_evt.h"
#include "abstractions/abstractions.h"

#define TAGB "BLE_INIT"

static uint8_t own_addr_type = BLE_OWN_ADDR_PUBLIC;

//sync cb
static void ble_app_on_sync(void) {
    int rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if(rc != 0) {
        mutex_log('E', TAGB, "Fatal: Failed to infer ble address type. rc=%d", rc);
        return;
    }

    mutexPrint(TAGB, "Hardware sync complete. Addr type verified.", 'I');
    ble_app_advertise();

}