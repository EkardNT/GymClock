#include "settings.h"
#include "debug.h"
#include <cstring>
#include <EEPROM.h>
#include <Arduino.h>

// If this is changed, make sure to also change the MARKER to a new value so that when the
// device starts up we'll detect that the old settings are out of date and clear them out.
struct Settings {
    char ssid[MAX_SSID_LENGTH + 1];
    char password[MAX_PASSWORD_LENGTH + 1];
};

// This is a simple magic value that is used to detect whether the EEPROM has been previously
// initialized or not.
const uint32_t MARKER = 0xFF0055AA;

Settings memorySettings;

void initSettings() {
    Debug.printf("Initializing EEPROM with length %d\n", SPI_FLASH_SEC_SIZE);
    EEPROM.begin(SPI_FLASH_SEC_SIZE);
    uint32_t markerVal = 0;
    EEPROM.get(0, markerVal);
    EEPROM.get(sizeof(uint32_t), memorySettings);
    if (markerVal != MARKER) {
        Debug.println("Writing default settings to EEPROM because marker value did not match \
            expected value");
        clearSettings();
        storeSettings();
    }
    Debug.println("Initialized in-memory settings from EEPROM");
}

void clearSettings() {
    Debug.println("Clearing in-memory settings to default values");
    memset(memorySettings.ssid, 0, MAX_SSID_LENGTH + 1);
    memset(memorySettings.password, 0, MAX_SSID_LENGTH + 1);
}

void storeSettings() {
    Debug.println("Writing in-memory settings to EEPROM");
    EEPROM.put(0, MARKER);
    EEPROM.put(sizeof(uint32_t), memorySettings);
    if (!EEPROM.commit()) {
        Debug.println("EEPROM.commit() failed, settings may not have been written to EEPROM");
    }
}

void dumpSettings() {
    Debug.println("Dumping all settings");
    Debug.printf("[setting] WiFi SSID: %s\n", memorySettings.ssid);
    Debug.printf("[setting] WiFi password: %s\n", memorySettings.password);
}

char const * getWifiSsid() {
    return memorySettings.ssid;
}

bool setWifiSsid(String &ssid) {
    if (ssid.length() > MAX_SSID_LENGTH) {
        Debug.printf("Unable to set WiFI SSID setting because the new length %d is greater than the \
            max length %d\n", ssid.length(), MAX_SSID_LENGTH);
        return false;
    }
    memset(memorySettings.ssid, 0, MAX_SSID_LENGTH + 1);
    strncpy(memorySettings.ssid, ssid.c_str(), ssid.length());
    Debug.printf("Set in-memory WiFi SSID setting to %s\n", ssid.c_str());
    return true;
}

char const * getWifiPassword() {
    return memorySettings.password;
}

bool setWifiPassword(String &password) {
    if (password.length() > MAX_PASSWORD_LENGTH) {
        Debug.printf("Unable to set WiFI password setting because the new length %d is greater than the \
            max length %d\n", password.length(), MAX_PASSWORD_LENGTH);
        return false;
    }
    memset(memorySettings.password, 0, MAX_PASSWORD_LENGTH + 1);
    strncpy(memorySettings.password, password.c_str(), password.length());
    Debug.printf("Set in-memory WiFi password setting to %s\n", password.c_str());
    return true;
}