#include <AceTime.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "debug.h"
#include "network.h"
#include "ntp.h"

// This serves the admin website.
WebServer webServer(80);
// This vends the "gymclock.local" domain name so we can navigate to the admin website
// by name instead of by IP address (once connected to the admin WiFi).
DNSServer dnsServer;

// Buffer for response body to reduce allocations.
String response("");

// Scratch buffer for temporary formatting.
const int FORMAT_BUF_SIZE = 256;
char tempFormatBuffer[FORMAT_BUF_SIZE] = {0};

// Formatting helpers which write into the tempFormatBuffer and return a pointer to it.
// Pointer is only valid until the next call to one of these formatting helpers, so never
// store it anywhere!
char * formatIntoTemp(IPAddress val);
char * formatIntoTemp(long val);
char * formatIntoTemp(long val, const char * formatSpecifier);
char * formatFloatIntoTemp(float val);
char * formatBoolIntoTemp(bool value);

// Route handlers.
void handleIndex();
void handleRebootConfirm();
void handleRebootSubmit();
void handleEnableUdpDebugConfirm();
void handleEnableUdpDebugSubmit();
void handleDisableUdpDebug();
void handleNotFound();

void startAdminServer(const IPAddress & accessPointIp) {
    dnsServer.start(53, "*", accessPointIp);
    Debug.println("DNS server started");

    webServer.on("/", handleIndex);
    webServer.on("/reboot", HTTP_GET, handleRebootConfirm);
    webServer.on("/reboot", HTTP_POST, handleRebootSubmit);
    webServer.on("/enableUdpDebug", HTTP_GET, handleEnableUdpDebugConfirm);
    webServer.on("/enableUdpDebug", HTTP_POST, handleEnableUdpDebugSubmit);
    webServer.on("/disableUdpDebug", HTTP_POST, handleDisableUdpDebug);
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
                    <form $DISABLE_UDP_DEBUG action='/disableUdpDebug' method='post'>\
                        <button type='submit'>Disable UDP Debug</button>\
                    </form>\
                    <h2>WiFi Access Point</h2>\
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
                    <h2>WiFi Station</h2>\
                    <ul>\
                        <li>Connected: $STA_CONNECTED</li>\
                        <li>SSID: $STA_SSID</li>\
                        <li>BSSID: $STA_BSSID</li>\
                        <li>Local IP: $STA_LOCAL_IP</li>\
                        <li>MAC Address: $STA_MAC_ADDRESS</li>\
                        <li>Subnet Mask: $STA_SUBNET_MASK</li>\
                        <li>Gateway IP: $STA_GATEWAY_IP</li>\
                        <li>RSSI: $STA_RSSI</li>\
                        <li>Auto Connect: $STA_AUTO_CONNECT</li>\
                        <li>Auto Reconnect: $STA_AUTO_RECONNECT</li>\
                    </ul>\
                    <h2>Request</h2>\
                    <ul>\
                        <li>Local IP: $REQ_LOCAL_IP</li>\
                        <li>Local Port: $REQ_LOCAL_PORT</li>\
                        <li>Remote IP: $REQ_REMOTE_IP</li>\
                        <li>Remote Port: $REQ_REMOTE_PORT</li>\
                    </ul>\
                    <h2>Time</h2>\
                    <ul>\
                        <li>Local Time: $LOCAL_YEAR/$LOCAL_MONTH/$LOCAL_DAY $LOCAL_HOUR:$LOCAL_MINUTE:$LOCAL_SECOND</li>\
                        <li>Epoch Seconds: $EPOCH_SECONDS</li>\
                        <li>Is NTP Time Set: $IS_NTP_TIME_SET</li>\
                        <li>Uptime (ms): $UPTIME</li>\
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
    response.replace(F("$STA_CONNECTED"), formatBoolIntoTemp(isStationConnected()));
    response.replace(F("$STA_SSID"), stationSSID());
    response.replace(F("$STA_BSSID"), stationBSSID());
    response.replace(F("$STA_LOCAL_IP"), formatIntoTemp(stationLocalIp()));
    response.replace(F("$STA_MAC_ADDRESS"), stationMacAddress());
    response.replace(F("$STA_SUBNET_MASK"), formatIntoTemp(stationSubnetMask()));
    response.replace(F("$STA_GATEWAY_IP"), formatIntoTemp(stationGatewayIP()));
    response.replace(F("$STA_RSSI"), formatIntoTemp(stationRSSI()));
    response.replace(F("$STA_AUTO_CONNECT"), formatBoolIntoTemp(isStationAutoConnect()));
    response.replace(F("$STA_AUTO_RECONNECT"), formatBoolIntoTemp(isStationAutoReconnect()));
    response.replace(F("$UPTIME"), formatIntoTemp(millis()));

    ace_time::ZonedDateTime localTime = getLocalTime();
    response.replace(F("$EPOCH_SECONDS"), formatIntoTemp(localTime.toEpochSeconds()));
    response.replace(F("$LOCAL_YEAR"), formatIntoTemp(localTime.year(), "%04d"));
    response.replace(F("$LOCAL_MONTH"), formatIntoTemp(localTime.month(), "%02d"));
    response.replace(F("$LOCAL_DAY"), formatIntoTemp(localTime.day(), "%02d"));
    response.replace(F("$LOCAL_HOUR"), formatIntoTemp(localTime.hour(), "%02d"));
    response.replace(F("$LOCAL_MINUTE"), formatIntoTemp(localTime.minute(), "%02d"));
    response.replace(F("$LOCAL_SECOND"), formatIntoTemp(localTime.second(), "%02d"));
    response.replace(F("$IS_NTP_TIME_SET"), formatBoolIntoTemp(isNtpTimeSet()));

    if (Debug.isUdpEnabled()) {
        response.replace(F("$ENABLE_UDP_DEBUG"), F("style='display:none;'"));
        response.replace(F("$DISABLE_UDP_DEBUG"), F("style='display:inline;'"));
    } else {
        response.replace(F("$ENABLE_UDP_DEBUG"), F("style='display:inline;'"));
        response.replace(F("$DISABLE_UDP_DEBUG"), F("style='display:none;'"));
    }
    webServer.send(200, "text/html", response);
}

void handleRebootConfirm() {
    Debug.println("handleRebootConfirm");

    response.remove(0);
    response.concat(F("\
        <html>\
            <head>\
                <title>GymClock Admin</title>\
                <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
                <link rel='stylesheet' href='/stylesheet.css'>\
                <style>\
                    .nav {\
                        background-color: red;\
                        color: white;\
                    }\
                </style>\
            </head>\
            <body>\
                <nav class='nav'>\
                    <h1>Reboot Device</h1>\
                </nav>\
                <p>Are you sure you want to reboot?</p>\
                <form style='display:inline;' action='/reboot' method='post'>\
                    <button type='submit'>Yes</button>\
                </form>\
                <form style='display:inline;' action='/' method='get'>\
                    <button type='submit'>No</button>\
                </form>\
            </body>\
        </html>"));
    webServer.send(200, "text/html", response);
}

void handleRebootSubmit() {
    Debug.println("handleRebootSubmit");

    response.remove(0);
    response.concat(F("\
        <html>\
            <head>\
                <title>GymClock Admin</title>\
                <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
                <meta http-equiv='refresh' content='10;url=/'>\
                <link rel='stylesheet' href='/stylesheet.css'>\
                <style>\
                    .nav {\
                        background-color: green;\
                        color: white;\
                    }\
                </style>\
            </head>\
            <body>\
                <nav class='nav'>\
                    <h1>Device Rebooting!</h1>\
                </nav>\
                <p>Device is rebooting <strong>now</strong>. Redirecting to index page in 10 seconds.</p>\
            </body>\
        </html>"));
    webServer.send(200, "text/html", response);

    Debug.println("Restarting ESP in response to admin request");
    ESP.restart();
}

void handleEnableUdpDebugConfirm() {
    Debug.println("handleEnableUdpDebugConfirm");

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
                    <h1>Enable UDP Debug</h1>\
                </nav>\
                <form action='/enableUdpDebug' method='post'>\
                    <p>Enter destination for UDP debug data.</p>\
                    <div><label>IP <input type='text' name='ip'></label></div>\
                    <div><label>Port <input type='text' name='port'></label></div>\
                    <button type='submit'>Enable UDP Debugging</button>\
                </form>\
            </body>\
        </html>"));
    webServer.send(200, "text/html", response);
}

void handleEnableUdpDebugSubmit() {
    Debug.println("handleEnableUdpDebugSubmit");

    IPAddress debugAddress;
    if (!debugAddress.fromString(webServer.arg("ip"))) {
        Debug.println("Bad IP address argument, displaying confirm screen again");
        handleEnableUdpDebugConfirm();
        return;
    }
    uint16_t debugPort = webServer.arg("port").toInt();
    if (debugPort == 0) {
        Debug.println("Bad port argument, displaying confirm screen again");
        handleEnableUdpDebugConfirm();
        return;
    }
    Debug.enableUdp(debugAddress, debugPort);
    Debug.printf("Enabled UDP debug to IP %s:%d\n", debugAddress, debugPort);
    
    response.remove(0);
    response.concat(F("\
        <html>\
            <head>\
                <title>GymClock Admin</title>\
                <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
                <meta http-equiv='refresh' content='3;url=/'>\
                <link rel='stylesheet' href='/stylesheet.css'>\
                <style>\
                    .nav {\
                        background-color: green;\
                        color: white;\
                    }\
                </style>\
            </head>\
            <body>\
                <nav class='nav'>\
                    <h1>UDP Debug Enabled</h1>\
                </nav>\
                <p>UDP debugging enabled to $DEBUG_ADDRESS:$DEBUG_PORT. Redirecting to index page in 3 seconds.</p>\
            </body>\
        </html>"));
    response.replace(F("$DEBUG_ADDRESS"), formatIntoTemp(debugAddress));
    response.replace(F("$DEBUG_PORT"), formatIntoTemp(debugPort));
    webServer.send(200, "text/html", response);
}

void handleDisableUdpDebug() {
    Debug.println("handleDisableUdpDebug");

    Debug.disableUdp();
    Debug.println("Disabled UDP debug");
    
    response.remove(0);
    response.concat(F("\
        <html>\
            <head>\
                <title>GymClock Admin</title>\
                <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
                <meta http-equiv='refresh' content='3;url=/'>\
                <link rel='stylesheet' href='/stylesheet.css'>\
                <style>\
                    .nav {\
                        background-color: green;\
                        color: white;\
                    }\
                </style>\
            </head>\
            <body>\
                <nav class='nav'>\
                    <h1>UDP Debug Disabled</h1>\
                </nav>\
                <p>UDP debugging disabled. Redirecting to index page in 3 seconds.</p>\
            </body>\
        </html>"));
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

char * formatIntoTemp(long val, const char * formatSpecifier) {
    snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, formatSpecifier, val);
    return tempFormatBuffer;
}

char * formatFloatIntoTemp(float val) {
    snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%.2f", val);
    return tempFormatBuffer;
}

char * formatBoolIntoTemp(bool val) {
    if (val) {
        snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%s", "true");
    } else {
        snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%s", "false");
    }
    return tempFormatBuffer;
}