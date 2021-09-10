#ifndef GYMCLOCK_NETWORK_H
#define GYMCLOCK_NETWORK_H

#include <Arduino.h>

void initializeNetwork();
void setStationCredentials(const char * ssid, const char * password);
void updateNetwork();

#endif