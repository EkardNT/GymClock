#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "admin_ui.h"
#include "debug.h"
#include "common.h"
#include "shared_ui.h"
#include "settings.h"

extern ESP8266WebServer adminServer;

// serveAdminUI is defined at the bottom of the file so I don't have to worry about forward
// declarations. Oh the joy of using a decades-old programming language...

void serveErrorPage(const __FlashStringHelper * errorMessage, const __FlashStringHelper * backLink) {
    WiFiClient client = adminServer.client();
    String body = "";
    body.reserve(2048);
    body.concat(F("\
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
                    <h1>GymClock Admin</h1>\
                </nav>\
                <div>\
                    <h2>Error</h2>\
                    <p>$ERROR_MESSAGE</p>\
                    <a href='$BACK_LINK'>Back</a>\
                </div>\
            </body>\
        </html>"));

    body.replace(F("$ERROR_MESSAGE"), errorMessage);
    body.replace(F("$BACK_LINK"), backLink);
    adminServer.send(200, F("text/html"), body);
}

void serveAdminIndex() {
    Debug.println(F("serveAdminIndex"));

    WiFiClient client = adminServer.client();
    String body = "";
    body.reserve(4096);
    body.concat(F("\
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
                    <div><a href='changeWiFi'>Change WiFi Settings</a></div>\
                    <div><a href='reboot'>Reboot</a></div>\
                    <div $ENABLE_UDP_DEBUG><a href='enableUdpDebug'>Enable UDP Debug</a></div>\
                    <div $DISABLE_UDP_DEBUG><a href='disableUdpDebug'>Disable UDP Debug</a></div>\
                    <div><a href='debugDump'>Dump to Debug</a></div>\
                    <h2>Station Info</h2>\
                    <ul>\
                        <li>IP: $WIFI_IP</li>\
                        <li>Subnet Mask: $WIFI_MASK</li>\
                        <li>Gateway IP: $WIFI_GATEWAY</li>\
                        <li>DNS IP: $WIFI_DNS</li>\
                        <li>Hostname: $WIFI_HOSTNAME</li>\
                        <li>RSSI: $WIFI_RSSI</li>\
                    </ul>\
                    <h2>Access Point Info</h2>\
                    <ul>\
                        <li>IP: $AP_IP</li>\
                        <li>MAC: $AP_MAC</li>\
                        <li>Stations: $AP_NUM_STATIONS</li>\
                    </ul>\
                    <h2>Request Info</h2>\
                    <ul>\
                        <li>Local IP: $REQ_LOCAL_IP</li>\
                        <li>Local Port: $REQ_LOCAL_PORT</li>\
                        <li>Remote IP: $REQ_REMOTE_IP</li>\
                        <li>Remote Port: $REQ_REMOTE_PORT</li>\
                    </ul>\
                    <h2>ESP Info</h2>\
                    <ul>\
                        <li>Free Heap: $FREE_HEAP</li>\
                        <li>Heap Fragmentation: $HEAP_FRAG</li>\
                        <li>Max Free Block Size: $MAX_FREE_BLOCK</li>\
                        <li>Chip Id: $CHIP_ID</li>\
                        <li>Core Version: $CORE_VERSION</li>\
                        <li>SDK Version: $SDK_VERSION</li>\
                        <li>CPU Frequency (MHz): $CPU_FREQ</li>\
                        <li>Sketch Size: $SKETCH_SIZE</li>\
                        <li>Free Sketch Space: $FREE_SKETCH_SPACE</li>\
                        <li>Flash Chip Id: $FLASH_ID</li>\
                        <li>Flash Chip Size: $FLASH_SIZE</li>\
                        <li>Flash Chip Real Size: $FLASH_REAL_SIZE</li>\
                        <li>Flash Chip Speed (Hz): $FLASH_SPEED</li>\
                        <li>Uptime (ms): $UPTIME</li>\
                    </ul>\
                </div>\
            </body>\
        </html>"));
    body.replace(F("$WIFI_IP"), formatIntoTemp(WiFi.localIP()));
    body.replace(F("$WIFI_MASK"), formatIntoTemp(WiFi.subnetMask()));
    body.replace(F("$WIFI_GATEWAY"), formatIntoTemp(WiFi.gatewayIP()));
    body.replace(F("$WIFI_DNS"), formatIntoTemp(WiFi.dnsIP()));
    body.replace(F("$WIFI_HOSTNAME"), WiFi.hostname());
    body.replace(F("$WIFI_RSSI"), formatFloatIntoTemp(WiFi.RSSI()));
    body.replace(F("$AP_IP"), formatIntoTemp(WiFi.softAPIP()));
    body.replace(F("$AP_MAC"), WiFi.softAPmacAddress());
    body.replace(F("$AP_NUM_STATIONS"), formatIntoTemp(WiFi.softAPgetStationNum()));
    body.replace(F("$REQ_LOCAL_IP"), formatIntoTemp(client.localIP()));
    body.replace(F("$REQ_LOCAL_PORT"), formatIntoTemp(client.localPort()));
    body.replace(F("$REQ_REMOTE_IP"), formatIntoTemp(client.remoteIP()));
    body.replace(F("$REQ_REMOTE_PORT"), formatIntoTemp(client.remotePort()));
    body.replace(F("$FREE_HEAP"), formatIntoTemp(ESP.getFreeHeap()));
    body.replace(F("$HEAP_FRAG"), formatIntoTemp(ESP.getHeapFragmentation()));
    body.replace(F("$MAX_FREE_BLOCK"), formatIntoTemp(ESP.getMaxFreeBlockSize()));
    body.replace(F("$CHIP_ID"), formatIntoTemp(ESP.getChipId()));
    body.replace(F("$CORE_VERSION"), ESP.getCoreVersion());
    body.replace(F("$SDK_VERSION"), ESP.getSdkVersion());
    body.replace(F("$CPU_FREQ"), formatIntoTemp((float) ESP.getCpuFreqMHz()));
    body.replace(F("$SKETCH_SIZE"), formatIntoTemp(ESP.getSketchSize()));
    body.replace(F("$FREE_SKETCH_SPACE"), formatIntoTemp(ESP.getFreeSketchSpace()));
    body.replace(F("$FLASH_ID"), formatIntoTemp(ESP.getFlashChipId()));
    body.replace(F("$FLASH_SIZE"), formatIntoTemp(ESP.getFlashChipSize()));
    body.replace(F("$FLASH_REAL_SIZE"), formatIntoTemp(ESP.getFlashChipRealSize()));
    body.replace(F("$FLASH_SPEED"), formatIntoTemp(ESP.getFlashChipSpeed()));
    body.replace(F("$UPTIME"), formatIntoTemp(millis()));
    if (Debug.isUdpEnabled()) {
        body.replace(F("$ENABLE_UDP_DEBUG"), F("style='display:none;'"));
        body.replace(F("$DISABLE_UDP_DEBUG"), F(""));
    } else {
        body.replace(F("$ENABLE_UDP_DEBUG"), F(""));
        body.replace(F("$DISABLE_UDP_DEBUG"), F("style='display:none;'"));
    }
    adminServer.send(200, F("text/html"), body);
}

void serveAdminReboot() {
    Debug.println(F("serveAdminReboot"));
    WiFiClient client = adminServer.client();
    String body = "";
    body.reserve(2048);
    body.concat(F("\
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
                    <h1>GymClock Admin</h1>\
                </nav>\
                <div>\
                    <h2>Confirm Restart</h2>\
                    <form action='rebootSubmit' method='post'>\
                        <p>Are you sure you want to reboot the system?<p>\
                        <p>You will need to reconnect to the admin network.</p>\
                        <p>If not, navigate back to the previous page.</p>\
                        <input type='submit' value='Reboot'>\
                    </form>\
                </div>\
            </body>\
        </html>"));
    adminServer.send(200, F("text/html"), body);
}

void serveAdminRebootSubmit() {
    Debug.println(F("serveAdminRebootSubmit"));
    WiFiClient client = adminServer.client();
    String body = "";
    body.reserve(2048);
    body.concat(F("\
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
                    <h1>GymClock Admin</h1>\
                </nav>\
                <div>\
                    <h2>Restarted</h2>\
                    <p>Adios, amigo. System going down for reboot NOW!</p>\
                </div>\
            </body>\
        </html>"));
    adminServer.send(200, F("text/html"), body);

    Debug.println(F("Rebooting due to admin request"));
    delay(100);
    ESP.restart();
}

void serveAdminChangeWiFi() {
    Debug.println(F("serveAdminChangeWiFi"));

    WiFiClient client = adminServer.client();
    String body = "";
    body.reserve(2048);
    body.concat(F("\
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
                    <h2>Change WiFi Settings</h2>\
                    <p>This page allows you to change the SSID and/or password that the clock\
                    will use to join the wireless network.</p>\
                    <p>Don't use passwords with single-quotes in them, because this form uses raw\
                    string interpolation which would break. Also open to injection attacks of course!</p>\
                    <p>SSID can be at most $MAX_SSID_LENGTH bytes. Password can be at most $MAX_PASSWORD_LENGTH bytes.</p>\
                    <form action='changeWiFiSubmit' method='post'>\
                        <div>\
                            <label for='ssid'>SSID</label>\
                            <input id='ssid' type='text' name='ssid' value='$CURRENT_SSID' required minlength='1' maxlength='$MAX_SSID_SIZE'>\
                        </div>\
                        <div>\
                            <label for='password'>Password</label>\
                            <input id='password' type='text' name='password' value='$CURRENT_PASSWORD' required minlength='1' maxlength='$MAX_PASSWORD_SIZE'>\
                        </div>\
                        <div>\
                            <label for='enableStaticIp'>Enable Static IP</label>\
                            <input id='enableStaticIp' type='checkbox' name='enableStaticIp' $STATIC_IP_CHECKED>\
                        </div>\
                        <div>\
                            <label for='staticIp'>Static IP</label>\
                            <input id='staticIp' type='text' name='staticIp' value='$CURRENT_STATIC_IP'>\
                        </div>\
                        <div>\
                            <label for='gatewayIp'>Default Gateway IP</label>\
                            <input id='gatewayIp' type='text' name='gatewayIp' value='$CURRENT_GATEWAY_IP'>\
                        </div>\
                        <div>\
                            <label for='subnetMask'>Subnet Mask</label>\
                            <input id='subnetMask' type='text' name='subnetMask' value='$CURRENT_SUBNET_MASK'>\
                        </div>\
                        <input type='submit' value='Change WiFi Settings'>\
                    </form>\
                </div>\
            </body>\
        </html>"));
    // Note that wifiSSID and wifiPassword are trivially empty strings if
    // they are not present.
    body.replace(F("$CURRENT_SSID"), getWifiSsid());
    body.replace(F("$CURRENT_PASSWORD"), getWifiPassword());
    body.replace(F("$MAX_SSID_LENGTH"), formatIntoTemp(MAX_SSID_LENGTH));
    body.replace(F("$MAX_PASSWORD_LENGTH"), formatIntoTemp(MAX_PASSWORD_LENGTH));
    body.replace(F("$STATIC_IP_CHECKED"), getEnableStaticIp() ? F("checked") : F(""));
    if (getEnableStaticIp()) {
        body.replace(F("$CURRENT_STATIC_IP"), formatIntoTemp(getStaticIp()));
        body.replace(F("$CURRENT_GATEWAY_IP"), formatIntoTemp(getGatewayIp()));
        body.replace(F("$CURRENT_SUBNET_MASK"), formatIntoTemp(getSubnetMask()));
    } else {
        // Default values.
        body.replace(F("$CURRENT_STATIC_IP"), F("192.168.0.X"));
        body.replace(F("$CURRENT_GATEWAY_IP"), F("192.168.0.1"));
        body.replace(F("$CURRENT_SUBNET_MASK"), F("255.255.255.0"));
    }
    adminServer.send(200, F("text/html"), body);
}

void serveAdminChangeWiFiSubmit() {
    Debug.println(F("serveAdminChangeWiFiSubmit"));

    bool enableStaticIp = adminServer.hasArg(F("enableStaticIp"));
    if (enableStaticIp) {
        IPAddress staticIp;
        IPAddress gatewayIp;
        IPAddress subnetMask;
        if (!hasArg(adminServer, F("staticIp")) || !staticIp.fromString(adminServer.arg(F("staticIp")))) {
            Debug.println("Bad or missing staticIp form parameter");
            serveErrorPage(F("Bad or missing static IP"), F("/changeWiFi"));
            return;
        }
        if (!hasArg(adminServer, F("gatewayIp")) || !gatewayIp.fromString(adminServer.arg(F("gatewayIp")))) {
            Debug.println("Bad or missing gatewayIp form parameter");
            serveErrorPage(F("Bad or missing gatewayIp IP"), F("/changeWiFi"));
            return;
        }
        if (!hasArg(adminServer, F("subnetMask")) || !subnetMask.fromString(adminServer.arg(F("subnetMask")))) {
            Debug.println("Bad or missing subnetMask form parameter");
            serveErrorPage(F("Bad or missing subnetMask IP"), F("/changeWiFi"));
            return;
        }
        setEnableStaticIp(true);
        setStaticIp(staticIp);
        setGatewayIp(gatewayIp);
        setSubnetMask(subnetMask);
    } else {
        setEnableStaticIp(false);
        setStaticIp(IPAddress(0, 0, 0, 0));
        setGatewayIp(IPAddress(0, 0, 0, 0));
        setSubnetMask(IPAddress(0, 0, 0, 0));
    }

    String newSSID = adminServer.arg(F("ssid"));
    if (!setWifiSsid(newSSID)) {
        Debug.println("Bad ssid form parameter");
        serveErrorPage(F("Bad network SSID"), F("/changeWiFi"));
        return;
    }

    String newPassword = adminServer.arg(F("password"));
    if (!setWifiPassword(newPassword)) {
        Debug.println("Bad password form parameter");
        serveErrorPage(F("Bad network password"), F("/changeWiFi"));
        return;
    }

    // Set successfully. Immediately write-through to persistent storage.
    storeSettings();

    WiFiClient client = adminServer.client();
    String body = "";
    body.reserve(2048);
    body.concat(F("\
        <html>\
            <head>\
                <title>GymClock Admin</title>\
                <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
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
                    <h1>GymClock Admin</h1>\
                </nav>\
                <div>\
                    <h2>Changed WiFi Settings</h2>\
                    <p>SSID changed to <strong>$CURRENT_SSID</strong>.</p>\
                    <p>Password changed to <strong>$CURRENT_PASSWORD</strong>.</p>\
                    <p>You will need to <a href='/reboot'>reboot the device</a> for the\
                    changes to take effect.</p>\
                    <a href='/'>Back to main menu</a>\
                </div>\
            </body>\
        </html>"));

    body.replace(F("$CURRENT_SSID"), newSSID);
    body.replace(F("$CURRENT_PASSWORD"), newPassword);
    body.replace(F("$MAX_SSID_LENGTH"), formatIntoTemp(MAX_SSID_LENGTH));
    body.replace(F("$MAX_PASSWORD_LENGTH"), formatIntoTemp(MAX_PASSWORD_LENGTH));
    adminServer.send(200, F("text/html"), body);
}

void serveAdminEnableUdpDebug() {
    Debug.println(F("serveAdminEnableUdpDebug"));
    serveSharedEnableUdpDebug(adminServer, F("GymClock Admin"));
}

void serveAdminEnableUdpDebugSubmit() {
    Debug.println(F("serveAdminEnableUdpDebugSubmit"));
    serveSharedEnableUdpDebugSubmit(adminServer, F("GymClock Admin"));
}

void serveAdminDisableUdpDebug() {
    Debug.println(F("serveAdminDisableUdpDebug"));
    serveSharedDisableUdpDebug(adminServer, F("GymClock Admin"));
}

void serveAdminDisableUdpDebugSubmit() {
    Debug.println(F("serveAdminDisableUdpDebugSubmit"));
    serveSharedDisableUdpDebugSubmit(adminServer, F("GymClock Admin"));
}

void serveAdminDebugDump() {
    Debug.println(F("serveAdminDebugDump"));
    Debug.dump();
    serveAdminIndex();
}

void serveAdminStylesheet() {
    Debug.println(F("serveAdminStylesheet"));
    serveSharedStylesheet(adminServer);
}

void setupAdminUI() {
    adminServer.on(F("/"), HTTP_GET, serveAdminIndex);
    adminServer.on(F("/reboot"), HTTP_GET, serveAdminReboot);
    adminServer.on(F("/rebootSubmit"), HTTP_POST, serveAdminRebootSubmit);
    adminServer.on(F("/changeWiFi"), HTTP_GET, serveAdminChangeWiFi);
    adminServer.on(F("/changeWiFiSubmit"), HTTP_POST, serveAdminChangeWiFiSubmit);
    adminServer.on(F("/enableUdpDebug"), HTTP_GET, serveAdminEnableUdpDebug);
    adminServer.on(F("/enableUdpDebug"), HTTP_POST, serveAdminEnableUdpDebugSubmit);
    adminServer.on(F("/disableUdpDebug"), HTTP_GET, serveAdminDisableUdpDebug);
    adminServer.on(F("/disableUdpDebug"), HTTP_POST, serveAdminDisableUdpDebugSubmit);
    adminServer.on(F("/debugDump"), HTTP_GET, serveAdminDebugDump);
    adminServer.on(F("/stylesheet.css"), serveAdminStylesheet);
}