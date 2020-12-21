#ifndef GYMCLOCK_SETTINGS_H
#define GYMCLOCK_SETTINGS_H

#include <IPAddress.h>

// Initializes the storage library, and loads settings from persistent storage into memory.
// This should be called before any other settings functions are called, and should be
// called only once at device startup.
void initSettings();
// Resets all settings to their default values. This only updates the in-memory settings -
// to persist the cleared settings, call storeSettings().
void clearSettings();
// Copy the current in-memory settings into persistent storage.
void storeSettings();
// Writes settings to the Debug output.
void dumpSettings();

// These lengths do not include the trailing null byte.
const int MAX_SSID_LENGTH = 31;
const int MAX_PASSWORD_LENGTH = 31;

char const * getWifiSsid();
bool setWifiSsid(String &ssid);

char const * getWifiPassword();
bool setWifiPassword(String &password);

bool getEnableStaticIp();
void setEnableStaticIp(bool enableStaticIp);

IPAddress getStaticIp();
void setStaticIp(const IPAddress &staticIp);

IPAddress getGatewayIp();
void setGatewayIp(const IPAddress &gatewayIp);

IPAddress getSubnetMask();
void setSubnetMask(const IPAddress &subnetMask);

#endif