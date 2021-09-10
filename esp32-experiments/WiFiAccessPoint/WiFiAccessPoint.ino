#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include "admin.h"
#include "debug.h"
#include "display.h"
#include "network.h"

const char STA_SSID[] = "Boulevard504-2GHz";
const char STA_PASSWORD[] = "foobar";

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    Debug.println();

    Debug.print("Setup running on core ");
    Debug.println(xPortGetCoreID());

    Debug.println("Initializing display");
    initializeDisplay();
    Debug.println("Initialized display");

    Debug.println("Initializing network");
    initializeNetwork();
    Debug.println("Initialized network");

    setStationCredentials(STA_SSID, STA_PASSWORD);
}

void loop() {
    updateNetwork();
}