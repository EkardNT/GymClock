#ifndef GYMCLOCK_ADMIN_H
#define GYMCLOCK_ADMIN_H

#include <Arduino.h>

void startAdminServer(const IPAddress & accessPointIp);
void stopAdminServer();
void updateAdminServer();

#endif