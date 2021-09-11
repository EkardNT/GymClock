#include "admin.h"
#include "debug.h"
#include "display.h"
#include "network.h"
#include "programs.h"

const char STA_SSID[] = "Boulevard504-2GHz";
const char STA_PASSWORD[] = "foobar";

void setup() {
    Serial.begin(115200);
    Debug.println("Starting setup");

    Debug.print("Setup running on core ");
    Debug.println(xPortGetCoreID());

    Debug.println("Initializing display");
    initializeDisplay();
    Debug.println("Initialized display");

    Debug.println("Initializing network");
    initializeNetwork();
    Debug.println("Initialized network");

    Debug.println("Setting station credentials");
    setStationCredentials(STA_SSID, STA_PASSWORD);
    
    Debug.println("Initializing programs");
    initializePrograms();
    
    Debug.println("Setup complete, entering loop");
}

void loop() {
    updateNetwork();
    updatePrograms();
}