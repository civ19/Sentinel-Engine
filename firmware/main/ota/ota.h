#pragma once

#include <stdint.h>

extern const uint8_t server_cert_pem_start[] asm("_binary_sentinel_cert_pem_start");
extern const uint8_t server_cert_pen_end[] asm("_binary_sentinel_cert_pem_end");

typedef struct {
    char buf[512];
    int len;
} http_response_ctx_t;

void init_ota_cloent(void);
void sentinel_ota_start(void);