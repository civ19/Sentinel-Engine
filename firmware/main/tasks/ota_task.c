#include "ota.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"

#include <string.h>
#include <stdio.h>

#include "abstractions/abstractions.h"

void perform_ota_task(void *pv);