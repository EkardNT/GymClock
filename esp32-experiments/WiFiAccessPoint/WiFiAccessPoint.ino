#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include "admin.h"
#include "debug.h"
#include "display.h"

const char AP_SSID[] = "GymClock";
// Change this to an actual password before flashing the sign?
const char AP_PASSWORD[] = "password";

const IPAddress AP_LOCAL_IP(192, 168, 1, 1);
const IPAddress AP_SUBNET_MASK(255, 255, 255, 0);

bool accessPointStarted = false;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    Debug.println();

    Debug.print("Setup running on core ");
    Debug.println(xPortGetCoreID());

    initializeDisplay();
    Debug.println("Initialized display");

    // You can remove the password parameter if you want the AP to be open.
    Debug.println("Disabling WiFi persistent settings mode");
    WiFi.persistent(false);
    Debug.println("Registering WiFi event handler");
    WiFi.onEvent(handleWiFiEvent);

    Debug.println("Requesting start of WiFi softAP");
    WiFi.softAP(AP_SSID, AP_PASSWORD);
}

void loop() {
    if (accessPointStarted) {
        updateAdminServer();
    }
}

void handleWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_READY:
            Debug.println("Handling ARDUINO_EVENT_WIFI_READY event");
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