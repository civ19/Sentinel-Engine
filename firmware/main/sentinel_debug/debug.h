#pragma once

#include "esp_core_dump.h"

void check_panic_data(esp_core_dump_summary_t *sum);
void activate_safe_mode(void);