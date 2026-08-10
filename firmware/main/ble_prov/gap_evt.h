#pragma once

#include "host/ble_hs.h"

void ble_app_advertise(void);
int ble_gap_event(struct ble_gap_event *event, void *arg);
