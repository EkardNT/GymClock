#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPUpdateServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <AceRoutine.h>
#include <AceTime.h>
#include "common.h"
#include "programs.h"

using namespace ace_routine;
using namespace ace_time;

const int FORMAT_BUF_SIZE = 256;
char tempFormatBuffer[FORMAT_BUF_SIZE] = {0};

// This web server allows administrative actions such as changing the saved WiFi ssid/password.
ESP8266WebServer adminServer(AP_LOCAL_IP, 80);
// This web server allows any user to remotely control the function of the clock.
ESP8266WebServer userServer(80);
// This vends the "gymclock.local" domain name so we don't have to access the admin server by IP.
DNSServer dnsServer;
// Services used to allow over-the-air updates.
ESP8266WebServer webUpdateServer(8266);
ESP8266HTTPUpdateServer httpUpdateServer;

// Used to setup and tear down the network services.
WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

// For retrieving the time.
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Tracks whether the display has connected to a wifi network in station mode.
bool networkActive = false;

// If the SSID and password are present in EEPROM, then these buffers will
// contain the null-terminated SSID and password. If the strlen of the
// buffers is 0, then that field is not present.
char wifiSSID[MAX_SSID_SIZE + 1] = {0};
char wifiPassword[MAX_PASSWORD_SIZE + 1] = {0};

byte displayState[NUM_DIGITS] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

struct InitProgram {
  void onBegin(unsigned long now);
  void onEnd();
  unsigned long update(unsigned long now);

  unsigned long startTime = 0;
};
struct TestProgram {
  void onBegin(unsigned long now);
  void onEnd();
  unsigned long update(unsigned long now);

  unsigned long startTime = 0;
};
struct ClockProgram {
  void onBegin(unsigned long now);
  void onEnd();
  unsigned long update(unsigned long now);

  bool format24 = false;
};
struct StopwatchProgram {
  void onBegin(unsigned long now);
  void onEnd();
  unsigned long update(unsigned long now);
};

InitProgram initProgram;
TestProgram testProgram;
ClockProgram clockProgram;
StopwatchProgram stopwatchProgram;

const int PROGRAM_INIT = 0;
const int PROGRAM_TEST = 1;
const int PROGRAM_CLOCK = 2;
const int PROGRAM_STOPWATCH = 3;
// Initialize previousProgram to an invalid program so that the first program's onBegin() is called.
int previousProgram = -1;
int currentProgram = PROGRAM_TEST;
unsigned long programNextUpdateTimeMillis = 0;
int programUpdateTick = 0;

TestProgramCo testProgramCo;
InitProgramCo initProgramCo;
ClockProgramCo clockProgramCo;
LoopProgramCo loopProgramCo;

BasicZoneManager<ZONE_MGR_CACHE_SIZE> zoneManager(
    zonedb::kZoneRegistrySize, zonedb::kZoneRegistry);

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing");
  Serial.setDebugOutput(true);

  initStoredSettings();

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP_STA);

  gotIpEventHandler = WiFi.onStationModeGotIP(networkInit);
  disconnectedEventHandler = WiFi.onStationModeDisconnected(networkStop);

  WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY, AP_SUBNET_MASK);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  if (strlen(wifiSSID) > 0 && strlen(wifiPassword) > 0) {
    Serial.printf("Trying to log on to WiFi network %s in station mode.\n", wifiSSID);
    WiFi.begin(wifiSSID, wifiPassword);
  } else {
    Serial.println("One or more of wifiSSID or wifiPassword not available from EEPROM, not logging on to any network yet.");
  }

  dnsServer.start(53, "gymclock.local", AP_LOCAL_IP);

  adminServer.on("/", HTTP_GET, serveAdminIndex);
  adminServer.on("/reboot", HTTP_GET, serveAdminReboot);
  adminServer.on("/rebootSubmit", HTTP_POST, serveAdminRebootSubmit);
  adminServer.on("/changeWiFi", HTTP_GET, serveAdminChangeWiFi);
  adminServer.on("/changeWiFiSubmit", HTTP_POST, serveAdminChangeWiFiSubmit);
  adminServer.begin();

  // Unfortunately the NTP/UDP request is apparently implemented as a blocking call, so it causes a noticable flicker.
  // Extending the interval to 3600000 (1 hour) is a hack to make this less noticable. If I ever reimplement the sign
  // using I2C and persistent lighting, it should be possible to lower this down to 60 seconds again.
  timeClient.setUpdateInterval(3600000);

  // Shift register control pins.
  pinMode(srDataPin, OUTPUT);
  pinMode(srStorageClockPin, OUTPUT);
  pinMode(srShiftClockPin, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(addrBitMap[i], OUTPUT);
  }

  // Set up the coroutine scheduler.
  initProgramCo.setupCoroutine(F("initProgramCo"));
  testProgramCo.setupCoroutine(F("testProgramCo"));
  clockProgramCo.setupCoroutine(F("clockProgramCo"));
  // loopProgramCo.setupCoroutine(F("loopProgramCo"));
  CoroutineScheduler::setup();

  Serial.println("Listing coroutines after setup");
  CoroutineScheduler::list(Serial);

  // Suspend everything but the init program.
  // WARNING: AceRoutine seems to have a bug if you suspend then resume here.
  suspendAll(&initProgramCo);

  Serial.println("Listing coroutines after suspending initial");
  CoroutineScheduler::list(Serial);

//  auto registrar = zoneManager.getRegistrar();
//  for (uint16_t i = 0; i < registrar.registrySize(); i++) {
//    BasicZone zoneInfo = registrar.getZoneInfoForIndex(i);
//    Serial.printf("Zone %s, short name %s, zone id %d\n", zoneInfo.name(), zoneInfo.shortName(), zoneInfo.zoneId());
//  }
}

// the loop function runs over and over again forever
void loop() {
  dnsServer.processNextRequest();
  adminServer.handleClient();
  if (networkActive) {
    userServer.handleClient();
    webUpdateServer.handleClient();
    timeClient.update();
  }
  renderDisplay();

  CoroutineScheduler::loop();

//  if (programUpdateTick == 64) {
//    updateProgram();
//    programUpdateTick = 0;
//  } else {
//    programUpdateTick += 1;
//  }
}

void initStoredSettings() {
  EEPROM.begin(EEPROM_SIZE);
  byte ssidSize = EEPROM.read(SSID_LEN_EEPROM_ADDR);
  if (ssidSize > 0 && ssidSize <= MAX_SSID_SIZE) {
    for (int i = 0; i < ssidSize; i++) {
      wifiSSID[i] = EEPROM.read(SSID_EEPROM_ADDR + i);
    }
  }
  Serial.printf("SSID size: %d, value: %s\n", strlen(wifiSSID), wifiSSID);
  byte passwordSize = EEPROM.read(PASSWORD_LEN_EEPROM_ADDR);
  if (passwordSize > 0 && passwordSize <= MAX_PASSWORD_SIZE) {
    for (int i = 0; i < passwordSize; i++) {
      wifiPassword[i] = EEPROM.read(PASSWORD_EEPROM_ADDR + i);
    }
  }
  Serial.printf("Password size: %d, value: %s\n", strlen(wifiPassword), wifiPassword);
}

void networkInit(const WiFiEventStationModeGotIP& event) {
  Serial.println("networkInit");

  userServer.on("/", HTTP_GET, serveUserIndex);
  userServer.on("/changeProgram", HTTP_GET, serveUserChangeProgram);
  userServer.on("/changeProgram/clock", HTTP_GET, serveUserChangeProgramClock);
  userServer.on("/changeProgram/clock", HTTP_POST, serveUserChangeProgramClockSubmit);
  userServer.on("/changeProgram/test", HTTP_GET, serveUserChangeProgramTest);
  userServer.on("/changeProgram/test", HTTP_POST, serveUserChangeProgramTestSubmit);
  userServer.begin();

  httpUpdateServer.setup(&webUpdateServer);
  webUpdateServer.begin();

  timeClient.begin();

  networkActive = true;
}

void networkStop(const WiFiEventStationModeDisconnected& event) {
  Serial.println("networkStop");
  userServer.stop();
  webUpdateServer.stop();
  timeClient.end();
  networkActive = false;
}

char * formatIntoTemp(IPAddress val) {
  snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%d.%d.%d.%d", val[0], val[1], val[2], val[3]);
  return tempFormatBuffer;
}

char * formatIntoTemp(long val) {
  snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%d", val);
  return tempFormatBuffer;
}

void serveAdminIndex() {
  WiFiClient client = adminServer.client();
  String body = "";
  body.reserve(4096);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock Admin</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
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
                <h2>Station Info</h2>\
                <ul>\
                    <li>IP: $WIFI_IP</li>\
                    <li>Subnet Mask: $WIFI_MASK</li>\
                    <li>Gateway IP: $WIFI_GATEWAY</li>\
                    <li>DNS IP: $WIFI_DNS</li>\
                    <li>Hostname: $WIFI_HOSTNAME</li>\
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
  adminServer.send(200, F("text/html"), body);
}

void serveAdminReboot() {
  WiFiClient client = adminServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock Admin</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
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
  WiFiClient client = adminServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock Admin</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
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

  Serial.println(F("Rebooting due to admin request"));
  delay(100);
  ESP.restart();
}

void serveAdminChangeWiFi() {
  WiFiClient client = adminServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock Admin</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
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
                <p>SSID can be at most $MAX_SSID_SIZE bytes. Password can be at most $MAX_PASSWORD_SIZE bytes.</p>\
                <form action='changeWiFiSubmit' method='post'>\
                    <div>\
                        <label for='ssid'>SSID</label>\
                        <input id='ssid' type='text' name='ssid' value='$CURRENT_SSID' required minlength='1' maxlength='$MAX_SSID_SIZE'>\
                    </div>\
                    <div>\
                        <label for='password'>Password</label>\
                        <input id='password' type='text' name='password' value='$CURRENT_PASSWORD' required minlength='1' maxlength='$MAX_PASSWORD_SIZE'>\
                    </div>\
                    <input type='submit' value='Change WiFi Settings'>\
                </form>\
            </div>\
        </body>\
    </html>"));
  // Note that wifiSSID and wifiPassword are trivially empty strings if
  // they are not present.
  body.replace(F("$CURRENT_SSID"), wifiSSID);
  body.replace(F("$CURRENT_PASSWORD"), wifiPassword);
  body.replace(F("$MAX_SSID_SIZE"), formatIntoTemp(MAX_SSID_SIZE));
  body.replace(F("$MAX_PASSWORD_SIZE"), formatIntoTemp(MAX_PASSWORD_SIZE));
  adminServer.send(200, F("text/html"), body);
}

void serveAdminChangeWiFiSubmit() {
  String newSSID = adminServer.arg(F("ssid"));
  String newPassword = adminServer.arg(F("password"));
  updateWiFiSettings(newSSID, newPassword);

  WiFiClient client = adminServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock Admin</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
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
                <a href='/'>Back to main menu</a>\
            </div>\
        </body>\
    </html>"));
  body.replace(F("$CURRENT_SSID"), wifiSSID);
  body.replace(F("$CURRENT_PASSWORD"), wifiPassword);
  body.replace(F("$MAX_SSID_SIZE"), formatIntoTemp(MAX_SSID_SIZE));
  body.replace(F("$MAX_PASSWORD_SIZE"), formatIntoTemp(MAX_PASSWORD_SIZE));
  adminServer.send(200, F("text/html"), body);
}

void serveUserIndex() {
  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <style>\
                nav {\
                    background-color: black;\
                    color: white;\
                }\
            </style>\
        </head>\
        <body>\
            <nav><h1>GymClock Main Menu</h1></nav>\
            <div>\
                <h2>Current Program</h2>\
                <p>Program: <strong>$CURRENT_PROGRAM</strong></p>\
            </div>\
            <div>\
                <h2>Actions</h2>\
                <div><a href='/changeProgram'>Change Program</a></div>\
            </div>\
        </body>\
    </html>"));
  switch (currentProgram) {
    case PROGRAM_INIT:
      body.replace(F("$CURRENT_PROGRAM"), F("Init"));
      break;
    case PROGRAM_TEST:
      body.replace(F("$CURRENT_PROGRAM"), F("Test"));
      break;
    case PROGRAM_CLOCK:
      body.replace(F("$CURRENT_PROGRAM"), F("Clock"));
      break;
    case PROGRAM_STOPWATCH:
      body.replace(F("$CURRENT_PROGRAM"), F("Stopwatch"));
      break;
  }
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgram() {
  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <style>\
                nav {\
                    background-color: black;\
                    color: white;\
                }\
            </style>\
        </head>\
        <body>\
            <nav><h1>Change Program</h1></nav>\
            <div>\
                <div><a href='/'>Back</a></div>\
                <div><a href='/changeProgram/clock'>Clock</a></div>\
                <div><a href='/changeProgram/stopwatch'>Stopwatch</a></div>\
                <div><a href='/changeProgram/test'>Test</a></div>\
            </div>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgramClock() {
  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
    <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <style>\
                nav {\
                    background-color: black;\
                    color: white;\
                }\
                form > * {\
                    display: block;\
                }\
            </style>\
        </head>\
        <body>\
            <nav><h1>Activate Clock</h1></nav>\
            <div><a href='/changeProgram'>Back</a></div>\
            <form method='post' action=''>\
                <label>\
                    Timezone\
                    <select name='timezoneId'>\
                        <option value='1'>America/Los Angeles</option>\
                    </select>\
                </label>\
                <label>\
                    24-Hour Format\
                    <input type='checkbox' name='format24'>\
                </label>\
                <input type='submit' value='Activate'>\
            </form>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgramClockSubmit() {
  String newTimezoneId = userServer.arg(F("timezoneId"));
  clockProgram.format24 = userServer.hasArg(F("format24"));
  changeProgram(PROGRAM_CLOCK);

  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <meta http-equiv='refresh' content='3;url=/'>\
            <style>\
                nav {\
                    background-color: green;\
                    color: white;\
                }\
            </style>\
        </head>\
        <body>\
            <nav><h1>Success</h1></nav>\
            <p><strong>Clock</strong> program activated. Returning to main menu.</p>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgramTest() {
  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
    <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <style>\
                nav {\
                    background-color: black;\
                    color: white;\
                }\
                form > * {\
                    display: block;\
                }\
            </style>\
        </head>\
        <body>\
            <nav><h1>Activate Test</h1></nav>\
            <div><a href='/changeProgram'>Back</a></div>\
            <form method='post' action=''>\
                <input type='submit' value='Activate'>\
            </form>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgramTestSubmit() {
  changeProgram(PROGRAM_TEST);

  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <meta http-equiv='refresh' content='3;url=/'>\
            <style>\
                nav {\
                    background-color: green;\
                    color: white;\
                }\
            </style>\
        </head>\
        <body>\
            <nav><h1>Success</h1></nav>\
            <p><strong>Test</strong> program activated. Returning to main menu.</p>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void updateWiFiSettings(String newSSID, String newPassword) {
  strncpy(wifiSSID, newSSID.c_str(), MAX_SSID_SIZE);
  strncpy(wifiPassword, newPassword.c_str(), MAX_PASSWORD_SIZE);
  byte ssidLen = newSSID.length();
  if (ssidLen > MAX_SSID_SIZE) {
    ssidLen = MAX_SSID_SIZE;
    Serial.println(F("Truncating new SSID to MAX_SSID_SIZE"));
  }
  Serial.print(F("New SSID length is "));
  Serial.println(ssidLen);
  Serial.print(F("New SSID is "));
  Serial.println(wifiSSID);
  byte passwordLen = newPassword.length();
  if (passwordLen > MAX_PASSWORD_SIZE) {
    passwordLen = MAX_PASSWORD_SIZE;
    Serial.println(F("Truncating new password to MAX_PASSWORD_SIZE"));
  }
  Serial.print(F("New password length is "));
  Serial.println(passwordLen);
  Serial.print(F("New password is "));
  Serial.println(wifiPassword);
  EEPROM.write(SSID_LEN_EEPROM_ADDR, ssidLen);
  for (int i = 0; i < ssidLen; i++) {
    EEPROM.write(SSID_EEPROM_ADDR + i, wifiSSID[i]);
  }
  for (int i = ssidLen; i < MAX_SSID_SIZE; i++) {
    EEPROM.write(SSID_EEPROM_ADDR + i, 0);
  }
  EEPROM.write(PASSWORD_LEN_EEPROM_ADDR, passwordLen);
  for (int i = 0; i < passwordLen; i++) {
    EEPROM.write(PASSWORD_EEPROM_ADDR + i, wifiPassword[i]);
  }
  for (int i = passwordLen; i < MAX_PASSWORD_SIZE; i++) {
    EEPROM.write(PASSWORD_EEPROM_ADDR + i, 0);
  }
  EEPROM.commit();

  Serial.println("Disconnecting WiFi.");
  WiFi.disconnect(false);
  if (strlen(wifiSSID) > 0 && strlen(wifiPassword) > 0) {
    Serial.println("Beginning WiFi.");
    WiFi.begin(wifiSSID, wifiPassword);
  }
}

void changeProgram(int newProgram) {
  previousProgram = currentProgram;
  currentProgram = newProgram;
  programNextUpdateTimeMillis = 0;
}

void updateProgram() {
  unsigned long nowMillis = millis();
  if (currentProgram != previousProgram) {
    switch (previousProgram) {
      case PROGRAM_INIT:
        initProgram.onEnd();
        break;
      case PROGRAM_TEST:
        testProgram.onEnd();
        break;
      case PROGRAM_CLOCK:
        clockProgram.onEnd();
        break;
      case PROGRAM_STOPWATCH:
        stopwatchProgram.onEnd();
        break;
      default:
        // This is normal on startup.
        break;
    }
    switch (currentProgram) {
      case PROGRAM_INIT:
        initProgram.onBegin(nowMillis);
        break;
      case PROGRAM_TEST:
        testProgram.onBegin(nowMillis);
        break;
      case PROGRAM_CLOCK:
        clockProgram.onBegin(nowMillis);
        break;
      case PROGRAM_STOPWATCH:
        stopwatchProgram.onBegin(nowMillis);
        break;
      default:
        Serial.println("Invalid current program!");
        break;
    }
    previousProgram = currentProgram;
  }
  if (nowMillis >= programNextUpdateTimeMillis) {
    switch (currentProgram) {
      case PROGRAM_INIT:
        programNextUpdateTimeMillis = initProgram.update(nowMillis);
        break;
      case PROGRAM_TEST:
        programNextUpdateTimeMillis = testProgram.update(nowMillis);
        break;
      case PROGRAM_CLOCK:
        programNextUpdateTimeMillis = clockProgram.update(nowMillis);
        break;
      case PROGRAM_STOPWATCH:
        programNextUpdateTimeMillis = stopwatchProgram.update(nowMillis);
        break;
      default:
        Serial.println("Invalid current program!");
        break;
    }
  }
}

void InitProgram::onBegin(unsigned long now) {
  this->startTime = now;
  Serial.println(F("InitProgram::onBegin()"));
}

void InitProgram::onEnd() {
  Serial.println(F("InitProgram::onEnd()"));
}

unsigned long InitProgram::update(unsigned long now) {
  Serial.println(F("InitProgram::update()"));
  unsigned long age = now - this->startTime;
  clearDisplay();
  if (age < 5000) {
    updateDigit(0, 'H', false);
    updateDigit(1, 'E', false);
    updateDigit(2, 'L', false);
    updateDigit(3, 'L', false);
    updateDigit(4, 'O', false);
  } else {
    if (networkActive) {
      updateDigit(0, 'N', false);
      updateDigit(1, 'E', false);
      updateDigit(2, 'T', false);
      updateDigit(4, 'O', false);
      updateDigit(5, 'N', false);
    } else {
      updateDigit(0, 'N', false);
      updateDigit(1, 'O', false);
      updateDigit(3, 'N', false);
      updateDigit(4, 'E', false);
      updateDigit(5, 'T', false);
    }
  }
  return now + 1000;
}

void TestProgram::onBegin(unsigned long now) {
  this->startTime = now;
  Serial.println(F("TestProgram::onBegin()"));
}

void TestProgram::onEnd() {
  Serial.println(F("TestProgram::onEnd()"));
}

unsigned long TestProgram::update(unsigned long now) {
  const int PERIOD = 3000;
  unsigned long age = now - this->startTime;
  if (age / PERIOD % 3 == 0) {
    for (int i = 0; i < NUM_DIGITS; i++) {
      updateDigit(i, ' ', false);
    }
    updateDigit(1, 'T', false);
    updateDigit(2, 'E', false);
    updateDigit(3, 'S', false);
    updateDigit(4, 'T', false);
  } else if (age / PERIOD % 3 == 1) {
    for (int i = 0; i < NUM_DIGITS; i++) {
      updateDigit(i, '0' + i, false);
    }
  } else {
    for (int i = 0; i < NUM_DIGITS; i++) {
      updateDigit(i, '8', false);
    }
  }
  return now + PERIOD;
}

void ClockProgram::onBegin(unsigned long now) {
  Serial.println(F("ClockProgram::onBegin()"));
  clearDisplay();
}

void ClockProgram::onEnd() {
  Serial.println(F("ClockProgram::onEnd()"));
}

unsigned long ClockProgram::update(unsigned long now) {
  if (!networkActive) {
    clearDisplay();
    updateDigit(0, 'N', false);
    updateDigit(1, 'O', false);
    updateDigit(3, 'N', false);
    updateDigit(4, 'E', false);
    updateDigit(5, 'T', false);
    return now + 2000;
  }

  unsigned long unixEpochSeconds = timeClient.getEpochTime();
  TimeZone localTz = zoneManager.createForZoneInfo(&zonedb::kZoneAmerica_Los_Angeles);
  ZonedDateTime localTime = ZonedDateTime::forUnixSeconds(unixEpochSeconds, localTz);

  uint8_t hour = localTime.hour();

  if (hour > 12 && !this->format24) {
    hour -= 12;
  }

  clearDisplay();
  show2DigitNumber(hour, 0);
  show2DigitNumber(localTime.minute(), 2);
  show2DigitNumber(localTime.second(), 4);
  show2DigitNumber(localTime.month(), 6);
  show2DigitNumber(localTime.day(), 8);

  return now + 250;
}

void StopwatchProgram::onBegin(unsigned long now) {
  Serial.println(F("StopwatchProgram::onBegin()"));
}

void StopwatchProgram::onEnd() {
  Serial.println(F("StopwatchProgram::onEnd()"));
}

unsigned long StopwatchProgram::update(unsigned long now) {
  Serial.println(F("StopwatchProgram::update()"));
  return now + 10000;
}
