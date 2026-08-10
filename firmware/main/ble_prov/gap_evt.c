#include "gap_evt.h"
#include <string.h>
#include "esp_log.h"
#include "abstractions/abstractions.h"

static uint8_t addr_type = BLE_OWN_ADDR_PUBLIC;

void ble_app_advertise(void) {
    struct ble_hs_adv_fields fields; //reset fields
    memset(&fields, 0, sizeof(fields));

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    const char *dev_name = "ESP32_Node";
    fields.name = (uint8_t *)dev_name;
    fields.name_len = strlen(dev_name);
    fields.name_is_complete = 1;

    int rc = ble_gap_adv_set_fields(&fields);
    if(rc != 0) {
        mutex_log('E', "BLE_GAP", "Error setting advertisement fields. rc=%d", rc);
        return;
    }

    struct ble_hs_adv_fields resp_fields;
    memset(&resp_fields, 0, sizeof(resp_fields));

    resp_fields.uuids128 = (const ble_uuid128_t *)BLE_UUID128_DECLARE(0xed, 0x71, 0xa1, 0x2e, 0xd3, 0x75, 0x49, 0x73,  0xad, 0x57, 
        0x17, 0x72, 0xab, 0x39, 0x10, 0x11);
    resp_fields.num_uuids128 = 1;
    resp_fields.uuids128_is_complete = 1;

    rc = ble_gap_adv_rsp_set_fields(&resp_fields);
    if(rc != 0) {
        mutex_log('E', "BLE_GAP", "Error setting scan response fields. rc=%d", rc);
        return;
    }

    struct ble_gap_adv_params ad_params;
    memset(&ad_params, 0, sizeof(ad_params));
    ad_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    ad_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &ad_params, ble_gap_event, NULL);
    if(rc != 0) {
        mutex_log('E', "BLE_GAP", "Error setting advertisement. rc=%d", rc);
    } else {
        mutex_log('I', "BLE_GAP", "Ble Advertising started successfully. Waiting for phone...");
    }
}

//this is for syncing host and controller(bt radio)
void ble_sync_radio(void) {
    int rc = ble_hs_id_infer_auto(0, &addr_type);
    if(rc != 0) {
        mutex_log('E', "BLE_GAP", "Fatal: Failed to infer ble address type. rc=%d", rc);
        return;
    }


    mutexPrint("BLE_GAP", "Hardware sync complete. Addr type verified.", 'I');
    ble_app_advertise();

}

int ble_gap_event(struct ble_gap_event *event, void* arg) { //gap event handler !

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if(event->connect.status == 0) mutex_log('I', "BLE_GAP", "BLE Smartphone Link Established. Ready for provisioning...");
            else {
                mutex_log('E', "BLE_GAP", "Event connection failed. Error status: %d. Restarting advertisement...", event->connect.status);
                ble_app_advertise();
            }
            return 0;
        break;

        case BLE_GAP_EVENT_DISCONNECT:
            mutex_log('W', "BLE_GAP", "Disconnected from client. Error status:%d", event->disconnect.reason);
            ble_app_advertise();
            return 0;
        
        break;

        case BLE_GAP_EVENT_MTU:
            mutex_log('I', "BLE_GAP", "MTU size successfully negotiated %d bytes", event->mtu.value);
            return 0;

        break;

        default:
            return 0;
        break;


    }
    
   
}