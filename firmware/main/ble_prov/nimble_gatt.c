#include "nimble_gatt.h"
#include <string.h>
#include "esp_log.h"
#include "host/ble_hs.h"

#include "abstractions/abstractions.h"
#include "tasks/w_task.h"
#include "tasks/server_task.h"



static const char *TAG = "BLE_GATT";

static int gatt_svr_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctx, void* arg);

static char wifi_ssid[MAX_SSID_LEN + 1]; //ssid, pass and broker containers
static char wifi_pass[MAX_PASS_LEN + 1];
static char server_ip[MAX_SVR_LEN + 1];

static const struct ble_gatt_chr_def prov_features[] = {
    { //wifi ssid
        .uuid = BLE_UUID128_DECLARE(0x2d, 0x71, 0xa1, 0x20, 0x23, 0x75, 0x49, 0xb3, 0xad, 0x57, 0x17, 0x72, 0xab, 0xe9, 0x10, 0x12),
        .access_cb = gatt_svr_access_cb,
            .flags = BLE_GATT_CHR_F_WRITE,
    },


    { //wifi pass
        .uuid = BLE_UUID128_DECLARE(0x2d, 0x51, 0xa1, 0x20, 0x53, 0x75, 0x49, 0x93, 
                        0xbd, 0x57, 0x17, 0x72, 0xab, 0x39, 0x10, 0x13),
        .access_cb = gatt_svr_access_cb,
        .flags = BLE_GATT_CHR_F_WRITE,
    },

    { //server uri
        .uuid = BLE_UUID128_DECLARE(0x1d, 0xaF, 0xbd, 0x8d, 0x81, 0x94, 0x91, 0xe8, 0xe1, 
            0xe2, 0xb1, 0xb2, 0x9e, 0xa3, 0x67, 0xf7),
        .access_cb = gatt_svr_access_cb,
        .flags = BLE_GATT_CHR_F_WRITE,
    },
    
    {.uuid = NULL} //signla to stop processing

};

const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID128_DECLARE(0xed, 0x71, 0xa1, 0x2e, 0xd3, 0x75, 0x49, 0x73, //prov service
                        0xad, 0x57, 0x17, 0x72, 0xab, 0x39, 0x10, 0x11),
        
        .characteristics = prov_features,
    },
    {.type = BLE_GATT_SVC_TYPE_END}
    
};

static int ssid_write(struct os_mbuf *om) {
    uint16_t len = OS_MBUF_PKTLEN(om);
    if (len > MAX_SSID_LEN) return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;

    memset(wifi_ssid, 0, sizeof(wifi_ssid));
    int rc = ble_hs_mbuf_to_flat(om, wifi_ssid, len, NULL);
    if (rc != 0) return BLE_ATT_ERR_UNLIKELY;

    mutex_log('I', TAG, "Successfully saved SSID: %s", wifi_ssid);
    return 0;
}

static int pass_write(struct os_mbuf *om) {
    uint16_t len = OS_MBUF_PKTLEN(om);
    if (len > MAX_PASS_LEN) return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;

    memset(wifi_pass, 0, sizeof(wifi_pass));
    int rc = ble_hs_mbuf_to_flat(om, wifi_pass, len, NULL);
    if (rc != 0) return BLE_ATT_ERR_UNLIKELY;

    mutex_log('I', TAG, "Successfully saved password securely.");
    
    trigger_wifi_provisioning(wifi_ssid, wifi_pass); 
    return 0;
}

static int server_ip_write(struct os_mbuf *om) {
    uint16_t len = OS_MBUF_PKTLEN(om); // getting len, clear buf, test rc, do to flat to write
    if(len > MAX_SVR_LEN) return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    memset(server_ip, 0, sizeof(server_ip));

    int rc = ble_hs_mbuf_to_flat(om, server_ip, len, NULL);
    if(rc != 0) return BLE_ATT_ERR_UNLIKELY;

    mutex_log('I', TAG, "Successfully saved mqtt broker uri securely.");
    trigger_server_prov(server_ip);
    return 0;


}

static int gatt_svr_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctx, void* arg) { 
    
    switch(ctx->op) {
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            
            if(ble_uuid_cmp(ctx->chr->uuid, BLE_UUID128_DECLARE(0x2d, 0x71, 0xa1, 0x20, 0x23, 0x75, 0x49, 0xb3, 0xad, 0x57, 
                0x17, 0x72, 0xab, 0xe9, 0x10, 0x12)) == 0) {
                return ssid_write(ctx->om);
            }
            
            if(ble_uuid_cmp(ctx->chr->uuid, BLE_UUID128_DECLARE(0x2d, 0x51, 0xa1, 0x20, 0x53, 0x75, 0x49, 0x93, 
                    0xbd, 0x57, 0x17, 0x72, 0xab, 0x39, 0x10, 0x13)) == 0) {
                return pass_write(ctx->om);
            }

            if(ble_uuid_cmp(ctx->chr->uuid, BLE_UUID128_DECLARE(0x1d, 0xaF, 0xbd, 0x8d, 0x81, 0x94, 0x91, 0xe8, 0xe1, 
            0xe2, 0xb1, 0xb2, 0x9e, 0xa3, 0x67, 0xf7)) == 0) return server_ip_write(ctx->om);

            return BLE_ATT_ERR_ATTR_NOT_FOUND;

        case BLE_GATT_ACCESS_OP_READ_CHR:
            return BLE_ATT_ERR_READ_NOT_PERMITTED;

        default:
            return BLE_ATT_ERR_UNLIKELY;
    }
}