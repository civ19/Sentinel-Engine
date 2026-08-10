#pragma once

#include "ota/ota.h"

#define SVR_CONN_BIT (1 << 1)

void perform_ota_task(void *pv);