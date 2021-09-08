#include <WebServer.h>
#include <DNSServer.h>
#include "debug.h"

WebServer webServer(80);
DNSServer dnsServer;

String response("");

const int FORMAT_BUF_SIZE = 256;
char tempFormatBuffer[FORMAT_BUF_SIZE] = {0};

char * formatIntoTemp(IPAddress val);
char * formatIntoTemp(long val);
char * formatFloatIntoTemp(float val);

void handleIndex();
void handleNotFound();

void startAdminServer(const IPAddress & accessPointIp) {
    dnsServer.start(53, "*", accessPointIp);
    Debug.println("DNS server started");

    webServer.on("/", handleIndex);
    webServer.onNotFound(handleNotFound);

    webServer.begin();
    Debug.println("Admin web server started");
}

void stopAdminServer() {
    webServer.close();
    Debug.println("Admin web server stopped");
    dnsServer.stop();
    Debug.println("DNS server stopped");
}

void updateAdminServer() {
    webServer.handleClient();
    dnsServer.processNextRequest();
}

void handleIndex() {
    Debug.println("handleIndex");
    WiFiClient client = webServer.client();

    response.remove(0);
    response.concat(F("\
        <html>\
            <head>\
                <title>GymClock Admin</title>\
                <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
                <link rel='stylesheet' href='/stylesheet.css'>\
                <style>\
                    .nav {\
                        background-color: black;\
                        color: white;\
                    }\
                </style>\
            </head>\
            <body>\
                <nav class='nav'>\
                    <h1>GymClock Admin</h1>\
                </nav>\
                <div>\
                    <h2>Actions</h2>\
                    <form style='display:inline;' action='/reboot' method='get'>\
                        <button type='submit'>Reboot</button>\
                    </form>\
                    <form $ENABLE_UDP_DEBUG action='/enableUdpDebug' method='get'>\
                        <button type='submit'>Enable UDP Debug</button>\
                    </form>\
                    <form $DISABLE_UDP_DEBUG action='/disableUdpDebug' method='get'>\
                        <button type='submit'>Disable UDP Debug</button>\
                    </form>\
                    <h2>Access Point</h2>\
                    <ul>\
                        <li>SSID: $AP_SSID</li>\
                        <li>IP: $AP_IP</li>\
                        <li>Broadcast IP: $AP_BROADCAST_IP</li>\
                        <li>Network Id: $AP_NETWORK_ID</li>\
                        <li>Subnet CIDR: $AP_SUBNET_CIDR</li>\
                        <li>Hostname: $AP_HOSTNAME</li>\
                        <li>MAC: $AP_MAC</li>\
                        <li>Stations: $AP_NUM_STATIONS</li>\
                    </ul>\
                    <h2>Request</h2>\
                    <ul>\
                        <li>Local IP: $REQ_LOCAL_IP</li>\
                        <li>Local Port: $REQ_LOCAL_PORT</li>\
                        <li>Remote IP: $REQ_REMOTE_IP</li>\
                        <li>Remote Port: $REQ_REMOTE_PORT</li>\
                    </ul>\
                    <h2>Memory</h2>\
                    <ul>\
                        <li>Free Heap: $FREE_HEAP</li>\
                        <li>Heap Size: $HEAP_SIZE</li>\
                        <li>Min Free Heap: $MIN_FREE_HEAP</li>\
                        <li>Max Alloc Heap: $MAX_ALLOC_HEAP</li>\
                        <li>PSRAM Size: $PSRAM_SIZE</li>\
                        <li>Free PSRAM: $FREE_PSRAM</li>\
                        <li>Minimum Free PSRAM: $MIN_FREE_PSRAM</li>\
                        <li>Max Alloc PSRAM: $MAX_ALLOC_PSRAM</li>\
                    </ul>\
                    <h2>ESP</h2>\
                    <ul>\
                        <li>Chip Revision: $CHIP_REVISION</li>\
                        <li>Chip Model: $CHIP_MODEL</li>\
                        <li>Chip Cores: $CHIP_CORES</li>\
                        <li>CPU Frequency (MHz): $CPU_FREQ</li>\
                        <li>Cycle Count: $CYCLE_COUNT</li>\
                        <li>SDK Version: $SDK_VERSION</li>\
                        <li>Sketch Size: $SKETCH_SIZE</li>\
                        <li>Free Sketch Space: $FREE_SKETCH_SPACE</li>\
                        <li>Flash Chip Size: $FLASH_SIZE</li>\
                        <li>Flash Chip Speed (Hz): $FLASH_SPEED</li>\
                        <li>Uptime (ms): $UPTIME</li>\
                    </ul>\
                </div>\
            </body>\
        </html>"));

    response.replace(F("$AP_IP"), formatIntoTemp(WiFi.softAPIP()));
    response.replace(F("$AP_MAC"), WiFi.softAPmacAddress());
    response.replace(F("$AP_NUM_STATIONS"), formatIntoTemp(WiFi.softAPgetStationNum()));
    response.replace(F("$AP_SSID"), WiFi.softAPSSID());
    response.replace(F("$AP_BROADCAST_IP"), formatIntoTemp(WiFi.softAPBroadcastIP()));
    response.replace(F("$AP_NETWORK_ID"), formatIntoTemp(WiFi.softAPNetworkID()));
    response.replace(F("$AP_SUBNET_CIDR"), formatIntoTemp(WiFi.softAPSubnetCIDR()));
    response.replace(F("$AP_HOSTNAME"), WiFi.softAPgetHostname());
    response.replace(F("$REQ_LOCAL_IP"), formatIntoTemp(client.localIP()));
    response.replace(F("$REQ_LOCAL_PORT"), formatIntoTemp(client.localPort()));
    response.replace(F("$REQ_REMOTE_IP"), formatIntoTemp(client.remoteIP()));
    response.replace(F("$REQ_REMOTE_PORT"), formatIntoTemp(client.remotePort()));
    response.replace(F("$FREE_HEAP"), formatIntoTemp(ESP.getFreeHeap()));
    response.replace(F("$HEAP_SIZE"), formatIntoTemp(ESP.getHeapSize()));
    response.replace(F("$MIN_FREE_HEAP"), formatIntoTemp(ESP.getMinFreeHeap()));
    response.replace(F("$MAX_ALLOC_HEAP"), formatIntoTemp(ESP.getMaxAllocHeap()));
    response.replace(F("$PSRAM_SIZE"), formatIntoTemp(ESP.getPsramSize()));
    response.replace(F("$FREE_PSRAM"), formatIntoTemp(ESP.getFreePsram()));
    response.replace(F("$MIN_FREE_PSRAM"), formatIntoTemp(ESP.getMinFreePsram()));
    response.replace(F("$MAX_ALLOC_PSRAM"), formatIntoTemp(ESP.getMaxAllocPsram()));
    response.replace(F("$CHIP_REVISION"), formatIntoTemp(ESP.getChipRevision()));
    response.replace(F("$CHIP_MODEL"), ESP.getChipModel());
    response.replace(F("$CHIP_CORES"), formatIntoTemp(ESP.getChipCores()));
    response.replace(F("$CYCLE_COUNT"), formatIntoTemp(ESP.getCycleCount()));
    response.replace(F("$SDK_VERSION"), ESP.getSdkVersion());
    response.replace(F("$CPU_FREQ"), formatIntoTemp((float) ESP.getCpuFreqMHz()));
    response.replace(F("$SKETCH_SIZE"), formatIntoTemp(ESP.getSketchSize()));
    response.replace(F("$FREE_SKETCH_SPACE"), formatIntoTemp(ESP.getFreeSketchSpace()));
    response.replace(F("$FLASH_SIZE"), formatIntoTemp(ESP.getFlashChipSize()));
    response.replace(F("$FLASH_SPEED"), formatIntoTemp(ESP.getFlashChipSpeed()));
    response.replace(F("$UPTIME"), formatIntoTemp(millis()));
    if (Debug.isUdpEnabled()) {
        response.replace(F("$ENABLE_UDP_DEBUG"), F("style='display:none;'"));
        response.replace(F("$DISABLE_UDP_DEBUG"), F("style='display:inline;'"));
    } else {
        response.replace(F("$ENABLE_UDP_DEBUG"), F("style='display:inline;'"));
        response.replace(F("$DISABLE_UDP_DEBUG"), F("style='display:none;'"));
    }
    webServer.send(200, "text/html", response);
}

void handleNotFound() {
    Debug.printf("handleNotFound for URI %s\n", webServer.uri());
    response.remove(0);
    response += "Not found!";
    webServer.send(404, "text/plain", response);
}

char * formatIntoTemp(IPAddress val) {
  snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%d.%d.%d.%d", val[0], val[1], val[2], val[3]);
  return tempFormatBuffer;
}

char * formatIntoTemp(long val) {
  snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%d", val);
  return tempFormatBuffer;
}

char * formatFloatIntoTemp(float val) {
  snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%.2f", val);
  return tempFormatBuffer;
}