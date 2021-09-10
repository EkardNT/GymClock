#include <Arduino.h>
#include <WiFi.h>
#include <string.h>
#include "admin.h"
#include "debug.h"
#include "network.h"
#include "ntp.h"

const char AP_SSID[] = "GymClock";
// Change this to an actual password before flashing the sign!
const char AP_PASSWORD[] = "password";

const IPAddress AP_LOCAL_IP(192, 168, 1, 1);
const IPAddress AP_SUBNET_MASK(255, 255, 255, 0);

char currentStationSSID[MAX_SSID_LEN] = {};
char currentStationPassword[MAX_PASSPHRASE_LEN] = {};

const char HOSTNAME[] = "gymclock";

bool accessPointStarted = false;
bool stationConnected = false;

void clearStationCredentials();
void handleWiFiEvent(WiFiEvent_t event);
void onAccessPointStarted();
void onAccessPointStopped();
void onStationConnectedWithIp();
void onStationLostIp();

void initializeNetwork() {
    clearStationCredentials();

    Debug.println("Registering WiFi event handler");
    WiFi.onEvent(handleWiFiEvent);
    Debug.println("Disabling WiFi persistent settings mode");
    WiFi.persistent(false);
    Debug.println("Setting WiFi mode to WIFI_AP_STA");
    WiFi.mode(WIFI_AP_STA);
    Debug.printf("Setting WiFi hostname to %s\n", HOSTNAME);
    WiFi.setHostname(HOSTNAME);
    Debug.println("Disabling WiFi auto-connect");
    WiFi.setAutoConnect(false);
    Debug.println("Enabling WiFi auto-reconnect");
    WiFi.setAutoReconnect(true);

    Debug.println("Requesting start of WiFi softAP");
    WiFi.softAP(AP_SSID, AP_PASSWORD);
}

void updateNetwork() {
    if (accessPointStarted) {
        updateAdminServer();
    }
    if (stationConnected) {
        updateNtp();
    }
}

void setStationCredentials(const char * ssid, const char * password) {
    clearStationCredentials();
    strncpy(currentStationSSID, ssid, MAX_SSID_LEN);
    currentStationSSID[MAX_SSID_LEN - 1] = 0;
    strncpy(currentStationPassword, password, MAX_PASSPHRASE_LEN);
    currentStationPassword[MAX_PASSPHRASE_LEN - 1] = 0;

    if (strlen(currentStationSSID) == 0) {
        Debug.println("Disconnecting WiFi STA mode because new SSID is an empty string");
        WiFi.disconnect(false, true);
    } else {
        Debug.printf("Start WiFi STA mode for SSID \"%s\" with password \"%s\"\n", currentStationSSID, currentStationPassword);
        WiFi.begin(currentStationSSID, currentStationPassword);
    }
}

void clearStationCredentials() {
    memset(currentStationSSID, 0, MAX_SSID_LEN);
    memset(currentStationPassword, 0, MAX_PASSPHRASE_LEN);
}

void handleWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_READY:
            Debug.println("Handling ARDUINO_EVENT_WIFI_READY event");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            Debug.println("Handling ARDUINO_EVENT_WIFI_SCAN_DONE event");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            Debug.println("Handling ARDUINO_EVENT_WIFI_STA_START event");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Debug.println("Handling ARDUINO_EVENT_WIFI_STA_STOP event");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Debug.println("Handling ARDUINO_EVENT_WIFI_STA_CONNECTED event");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Debug.println("Handling ARDUINO_EVENT_WIFI_STA_DISCONNECTED event");
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Debug.println("Handling ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE event");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Debug.println("Handling ARDUINO_EVENT_WIFI_STA_GOT_IP event");
            onStationConnectedWithIp();
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Debug.println("Handling ARDUINO_EVENT_WIFI_STA_LOST_IP event");
            onStationLostIp();
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Debug.println("Handling ARDUINO_EVENT_WIFI_AP_START event");
            onAccessPointStarted();
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Debug.println("Handling ARDUINO_EVENT_WIFI_AP_STOP event");
            onAccessPointStopped();
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Debug.println("Received ARDUINO_EVENT_WIFI_AP_STACONNECTED event");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Debug.println("Received ARDUINO_EVENT_WIFI_AP_STADISCONNECTED event");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Debug.println("Received ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED event");
            break;
        default:
            Debug.printf("Unexpected WiFi event with numeric value %d\n", event);
            break;
    }
}

void onAccessPointStarted() {
    WiFi.softAPConfig(AP_LOCAL_IP, AP_LOCAL_IP, AP_SUBNET_MASK);

    Debug.print("Access point SSID: ");
    Debug.println(WiFi.softAPSSID());
    Debug.print("Access point IP: ");
    Debug.println(WiFi.softAPIP());
    Debug.print("Access point broadcast IP: ");
    Debug.println(WiFi.softAPBroadcastIP());
    Debug.print("Access point network IP: ");
    Debug.println(WiFi.softAPNetworkID());
    Debug.print("Access point subnet CIDR: ");
    Debug.println(WiFi.softAPSubnetCIDR());
    Debug.print("Access point hostname: ");
    Debug.println(WiFi.softAPgetHostname());
    Debug.print("Access point MAC address: ");
    Debug.println(WiFi.softAPmacAddress());

    startAdminServer(AP_LOCAL_IP);
    accessPointStarted = true;
}

void onAccessPointStopped() {
    stopAdminServer();
    accessPointStarted = false;
}

void onStationConnectedWithIp() {
    Debug.println("WiFi STA now connected with IP address");
    initializeNtp();
    stationConnected = true;
}

void onStationLostIp() {
    Debug.println("WiFi STA disconnected, no IP address :(");
    shutdownNtp();
    stationConnected = false;
}