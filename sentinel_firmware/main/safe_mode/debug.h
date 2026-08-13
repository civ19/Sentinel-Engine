#pragma once

#include "esp_core_dump.h"

void check_panic_data(esp_core_dump_summary_t *sum);
void activate_safe_mode(void);
void esp_wake_reason(void);
void esp_rst_reason(void);
void init_console(void);
void sync_time(void);