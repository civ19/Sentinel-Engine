#pragma once

#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "esp_http_client.h"
#include "esp_app_format.h"
#include "esp_err.h"

extern const uint8_t server_cert_pem_start[] asm("_binary_sentinel_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_sentinel_cert_pem_end");


void updated_check(esp_https_ota_handle_t handle, int status);
int get_status_code();
char *get_hash_header();

esp_err_t validate_img_header(esp_app_desc_t *new_app_info);
esp_err_t _http_event_handler(esp_http_client_event_t* evt);
esp_err_t init_ota(esp_https_ota_handle_t *out_handle);
