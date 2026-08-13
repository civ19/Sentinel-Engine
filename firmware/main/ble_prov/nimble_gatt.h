#pragma once

#include "host/ble_hs.h" 
#include "services/gatt/ble_svc_gatt.h"

#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
#define MAX_SVR_LEN 64

extern char wifi_ssid[MAX_SSID_LEN + 1]; //ssid, pass and server ip containers
extern char wifi_pass[MAX_PASS_LEN + 1];
extern char server_ip[MAX_SVR_LEN + 1];

extern const struct ble_gatt_svc_def gatt_svr_svcs[];

