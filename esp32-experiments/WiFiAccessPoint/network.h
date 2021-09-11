#ifndef GYMCLOCK_NETWORK_H
#define GYMCLOCK_NETWORK_H

#include <Arduino.h>

void initializeNetwork();
void setStationCredentials(const char * ssid, const char * password);
void updateNetwork();

bool isStationConnected();
bool isStationAutoConnect();
bool isStationAutoReconnect();
IPAddress stationLocalIp();
String stationMacAddress();
IPAddress stationSubnetMask();
IPAddress stationGatewayIP();
String stationSSID();
String stationBSSID();
int8_t stationRSSI();

#endif