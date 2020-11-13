#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPUpdateServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <IPAddress.h>
#include <AceRoutine.h>
#include <AceTime.h>
#include "common.h"
#include "programs.h"
#include "sound.h"
#include "debug.h"
#include "admin_ui.h"
#include "shared_ui.h"

using namespace ace_routine;
using namespace ace_time;

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

// All coroutine instances.
TestProgram testProgram;
InitProgram initProgram;
ClockProgram clockProgram;
CountdownProgram countdownProgram;
StopwatchProgram stopwatchProgram;
ScoredCountdownProgram scoredCountdownProgram;
SoundRoutine soundRoutine;

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
  WiFi.setAutoReconnect(true);
  if (strlen(wifiSSID) > 0 && strlen(wifiPassword) > 0) {
    Debug.printf("Trying to log on to WiFi network %s in station mode.\n", wifiSSID);
    WiFi.begin(wifiSSID, wifiPassword);
  } else {
    Debug.println("One or more of wifiSSID or wifiPassword not available from EEPROM, not logging on to any network yet.");
  }

  dnsServer.start(53, F("gymclock.local"), AP_LOCAL_IP);

  setupAdminUI();
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
  pinMode(tonePin, OUTPUT);

  // Set up the coroutine scheduler.
  initProgram.setupCoroutine(F("init"));
  testProgram.setupCoroutine(F("test"));
  clockProgram.setupCoroutine(F("clock"));
  countdownProgram.setupCoroutine(F("countdown"));
  stopwatchProgram.setupCoroutine(F("stopwatch"));
  scoredCountdownProgram.setupCoroutine(F("scoredCountdown"));
  soundRoutine.setupCoroutine(F("sound"));
  CoroutineScheduler::setup();

  Debug.println("Listing coroutines after setup");
  CoroutineScheduler::list(Debug);

  // Suspend every program except the init program. Ideally we would call changeProgram(PROGRAM_INIT)
  // here instead of suspendAll, but that causes a crash reboot loop because AceRoutine has
  // some bug where the scheduler's linked list gets messed up if you make some combination
  // of suspend/resume calls here. See https://github.com/bxparks/AceRoutine/issues/19
  suspendAll(PROGRAM_INIT);

  Debug.println("Listing coroutines after suspending initial");
  CoroutineScheduler::list(Debug);

//  auto registrar = zoneManager.getRegistrar();
//  for (uint16_t i = 0; i < registrar.registrySize(); i++) {
//    BasicZone zoneInfo = registrar.getZoneInfoForIndex(i);
//    Debug.printf("Zone %s, short name %s, zone id %d\n", zoneInfo.name(), zoneInfo.shortName(), zoneInfo.zoneId());
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
}

void initStoredSettings() {
  EEPROM.begin(EEPROM_SIZE);
  byte ssidSize = EEPROM.read(SSID_LEN_EEPROM_ADDR);
  if (ssidSize > 0 && ssidSize <= MAX_SSID_SIZE) {
    for (int i = 0; i < ssidSize; i++) {
      wifiSSID[i] = EEPROM.read(SSID_EEPROM_ADDR + i);
    }
  }
  Debug.printf("SSID size: %d, value: %s\n", strlen(wifiSSID), wifiSSID);
  byte passwordSize = EEPROM.read(PASSWORD_LEN_EEPROM_ADDR);
  if (passwordSize > 0 && passwordSize <= MAX_PASSWORD_SIZE) {
    for (int i = 0; i < passwordSize; i++) {
      wifiPassword[i] = EEPROM.read(PASSWORD_EEPROM_ADDR + i);
    }
  }
  Debug.printf("Password size: %d, value: %s\n", strlen(wifiPassword), wifiPassword);
}

void networkInit(const WiFiEventStationModeGotIP& event) {
  Debug.println("networkInit");

  userServer.on(F("/"), HTTP_GET, serveUserIndex);
  userServer.on(F("/changeProgram"), HTTP_GET, serveUserChangeProgram);
  userServer.on(F("/changeProgram/clock"), HTTP_GET, serveUserChangeProgramClock);
  userServer.on(F("/changeProgram/clock"), HTTP_POST, serveUserChangeProgramClockSubmit);
  userServer.on(F("/changeProgram/test"), HTTP_GET, serveUserChangeProgramTest);
  userServer.on(F("/changeProgram/test"), HTTP_POST, serveUserChangeProgramTestSubmit);
  userServer.on(F("/changeProgram/countdown"), HTTP_GET, serveUserChangeProgramCountdown);
  userServer.on(F("/changeProgram/countdown"), HTTP_POST, serveUserChangeProgramCountdownSubmit);
  userServer.on(F("/changeProgram/stopwatch"), HTTP_GET, serveUserChangeProgramStopwatch);
  userServer.on(F("/changeProgram/stopwatch"), HTTP_POST, serveUserChangeProgramStopwatchSubmit);
  userServer.on(F("/changeProgram/scoredCountdown"), HTTP_GET, serveUserChangeProgramScoredCountdown);
  userServer.on(F("/changeProgram/scoredCountdown"), HTTP_POST, serveUserChangeProgramScoredCountdownSubmit);
  userServer.on(F("/reboot"), HTTP_GET, serveUserReboot);
  userServer.on(F("/rebootSubmit"), HTTP_POST, serveUserRebootSubmit);
  userServer.on(F("/enableUdpDebug"), HTTP_GET, serveUserEnableUdpDebug);
  userServer.on(F("/enableUdpDebug"), HTTP_POST, serveUserEnableUdpDebugSubmit);
  userServer.on(F("/disableUdpDebug"), HTTP_GET, serveUserDisableUdpDebug);
  userServer.on(F("/disableUdpDebug"), HTTP_POST, serveUserDisableUdpDebugSubmit);
  userServer.on(F("/scoredCountdownIncrementLeft"), HTTP_GET, serveUserScoredCountdownIncrementLeft);
  userServer.on(F("/scoredCountdownIncrementRight"), HTTP_GET, serveUserScoredCountdownIncrementRight);
  userServer.on(F("/scoredCountdownDecrementLeft"), HTTP_GET, serveUserScoredCountdownDecrementLeft);
  userServer.on(F("/scoredCountdownDecrementRight"), HTTP_GET, serveUserScoredCountdownDecrementRight);
  userServer.on(F("/debugDump"), HTTP_GET, serveUserDebugDump);
  userServer.on(F("/stylesheet.css"), serveUserStylesheet);
  userServer.begin();

  httpUpdateServer.setup(&webUpdateServer);
  webUpdateServer.begin();

  timeClient.begin();

  networkActive = true;
}

void networkStop(const WiFiEventStationModeDisconnected& event) {
  Debug.println("networkStop");
  userServer.stop();
  webUpdateServer.stop();
  timeClient.end();
  networkActive = false;
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
            <link rel='stylesheet' href='/stylesheet.css'>\
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
                <div $SCORED_COUNTDOWN_DISPLAY>\
                    <a href='/scoredCountdownIncrementLeft'>+ Left Score</a>\
                    <a href='/scoredCountdownIncrementRight'>+ Right Score</a>\
                    <a href='/scoredCountdownDecrementLeft'>- Left Score</a>\
                    <a href='/scoredCountdownDecrementRight'>- Right Score</a>\
                </div>\
            </div>\
            <div>\
                <h2>Actions</h2>\
                <div><a href='/changeProgram'>Change Program</a></div>\
            </div>\
            <div>\
                <h2>WiFi Info</h2>\
                <ul>\
                    <li>IP: $WIFI_IP</li>\
                    <li>Subnet Mask: $WIFI_MASK</li>\
                    <li>Gateway IP: $WIFI_GATEWAY</li>\
                    <li>DNS IP: $WIFI_DNS</li>\
                    <li>Hostname: $WIFI_HOSTNAME</li>\
                    <li>RSSI: $WIFI_RSSI</li>\
                </ul>\
            </div>\
            <div>\
                <h2>Debug</h2>\
                <div><a href='/reboot'>Reboot Sign</a></div>\
                <div $ENABLE_UDP_DEBUG><a href='/enableUdpDebug'>Enable UDB Debug</a></div>\
                <div $DISABLE_UDP_DEBUG><a href='/disableUdpDebug'>Disable UDB Debug</a></div>\
                <div><a href='/debugDump'>Dump to Debug</a></div>\
            </div>\
        </body>\
    </html>"));
  switch (currentProgram()) {
    case PROGRAM_INIT:
      body.replace(F("$CURRENT_PROGRAM"), F("Init"));
      break;
    case PROGRAM_TEST:
      body.replace(F("$CURRENT_PROGRAM"), F("Test"));
      break;
    case PROGRAM_CLOCK:
      body.replace(F("$CURRENT_PROGRAM"), F("Clock"));
      break;
    case PROGRAM_COUNTDOWN:
      body.replace(F("$CURRENT_PROGRAM"), F("Countdown"));
      break;
    case PROGRAM_STOPWATCH:
      body.replace(F("$CURRENT_PROGRAM"), F("Stopwatch"));
      break;
    case PROGRAM_SCORED_COUNTDOWN:
      body.replace(F("$CURRENT_PROGRAM"), F("Scored Countdown"));
      break;
    default:
      body.replace(F("$CURRENT_PROGRAM"), F("Unknown"));
  }
  body.replace(F("$WIFI_IP"), formatIntoTemp(WiFi.localIP()));
  body.replace(F("$WIFI_MASK"), formatIntoTemp(WiFi.subnetMask()));
  body.replace(F("$WIFI_GATEWAY"), formatIntoTemp(WiFi.gatewayIP()));
  body.replace(F("$WIFI_DNS"), formatIntoTemp(WiFi.dnsIP()));
  body.replace(F("$WIFI_HOSTNAME"), WiFi.hostname());
  body.replace(F("$WIFI_RSSI"), formatFloatIntoTemp(WiFi.RSSI()));
  if (Debug.isUdpEnabled()) {
    body.replace(F("$ENABLE_UDP_DEBUG"), F("style='display:none;'"));
    body.replace(F("$DISABLE_UDP_DEBUG"), F(""));
  } else {
    body.replace(F("$ENABLE_UDP_DEBUG"), F(""));
    body.replace(F("$DISABLE_UDP_DEBUG"), F("style='display:none;'"));
  }
  if (currentProgram() == PROGRAM_SCORED_COUNTDOWN) {
    body.replace(F("$SCORED_COUNTDOWN_DISPLAY"), F(""));
  } else {
    body.replace(F("$SCORED_COUNTDOWN_DISPLAY"), F("style='display:none;'"));
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
            <link rel='stylesheet' href='/stylesheet.css'>\
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
                <div><a href='/changeProgram/countdown'>Countdown</a></div>\
                <div><a href='/changeProgram/scoredCountdown'>Scored Countdown</a></div>\
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
            <link rel='stylesheet' href='/stylesheet.css'>\
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
            <link rel='stylesheet' href='/stylesheet.css'>\
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
            <link rel='stylesheet' href='/stylesheet.css'>\
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
            <link rel='stylesheet' href='/stylesheet.css'>\
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

void serveUserChangeProgramCountdown() {
  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
    <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <link rel='stylesheet' href='/stylesheet.css'>\
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
            <nav><h1>Activate Countdown</h1></nav>\
            <div><a href='/changeProgram'>Back</a></div>\
            <form method='post' action=''>\
                <div>\
                    <strong>Sets</strong>\
                    <div><label>Count <input type='number' name='sets' value='1' min='1' max='99' required></label></div>\
                </div>\
                <div>\
                    <strong>Set Time</strong>\
                    <div><label>Minutes <input type='number' name='setDurationMinutes' placeholder='0' min='0' max='60'></label></div>\
                    <div><label>Seconds <input type='number' name='setDurationSeconds' placeholder='0' min='0' max='60'></label></div>\
                </div>\
                <div>\
                    <strong>Rest Time</strong>\
                    <div><label>Seconds <input type='number' name='restSeconds' placeholder='0' min='0' max='99'></label></div>\
                </div>\
                <div>\
                    <strong>Preparation Time</strong>\
                    <div><label>Seconds <input type='number' name='readySeconds' value='5' min='0' max='10' required></label></div>\
                </div>\
                <input type='submit' value='Activate'>\
            </form>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgramCountdownSubmit() {
  int readySeconds = userServer.arg(F("readySeconds")).toInt();
  readySeconds = constrain(readySeconds, 0, 10);
  int sets = userServer.arg(F("sets")).toInt();
  sets = constrain(sets, 1, 99);
  unsigned long setDurationMinutes = userServer.hasArg(F("setDurationMinutes"))
    ? userServer.arg(F("setDurationMinutes")).toInt()
    : 0;
  setDurationMinutes = constrain(setDurationMinutes, 0, 60);
  unsigned long setDurationSeconds = userServer.hasArg(F("setDurationSeconds"))
    ? userServer.arg(F("setDurationSeconds")).toInt()
    : 0;
  setDurationSeconds = constrain(setDurationSeconds, 0, 60);
  unsigned long restSeconds = userServer.hasArg(F("restSeconds"))
    ? userServer.arg(F("restSeconds")).toInt()
    : 0;
  restSeconds = constrain(restSeconds, 0, 99);

  unsigned long setDurationMillis = setDurationMinutes * MILLIS_PER_MINUTE
    + setDurationSeconds * MILLIS_PER_SECOND;

  countdownProgram.sets = sets;
  countdownProgram.readySeconds = readySeconds;
  countdownProgram.setDurationMillis = setDurationMillis;
  countdownProgram.restSeconds = restSeconds;

  changeProgram(PROGRAM_COUNTDOWN);

  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <meta http-equiv='refresh' content='3;url=/'>\
            <link rel='stylesheet' href='/stylesheet.css'>\
            <style>\
                nav {\
                    background-color: green;\
                    color: white;\
                }\
            </style>\
        </head>\
        <body>\
            <nav><h1>Success</h1></nav>\
            <p><strong>Countdown</strong> program activated. Returning to main menu.</p>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgramStopwatch() {
  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
    <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <link rel='stylesheet' href='/stylesheet.css'>\
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
            <nav><h1>Activate Stopwatch</h1></nav>\
            <div><a href='/changeProgram'>Back</a></div>\
            <form method='post' action=''>\
                <div>\
                    <strong>Beep Interval</strong> (leave 0 to disable)\
                    <div><label>Minutes <input type='number' name='beepIntervalMinutes' placeholder='0' min='0' max='60'></label></div>\
                    <div><label>Seconds <input type='number' name='beepIntervalSeconds' placeholder='0' min='0' max='60'></label></div>\
                </div>\
                <input type='submit' value='Activate'>\
            </form>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgramStopwatchSubmit() {
  unsigned long beepIntervalMinutes = userServer.hasArg(F("beepIntervalMinutes"))
    ? userServer.arg(F("beepIntervalMinutes")).toInt()
    : 0;
  beepIntervalMinutes = constrain(beepIntervalMinutes, 0, 60);
  unsigned long beepIntervalSeconds = userServer.hasArg(F("beepIntervalSeconds"))
    ? userServer.arg(F("beepIntervalSeconds")).toInt()
    : 0;
  beepIntervalSeconds = constrain(beepIntervalSeconds, 0, 60);

  unsigned long beepIntervalMillis = beepIntervalMinutes * MILLIS_PER_MINUTE
    + beepIntervalSeconds * MILLIS_PER_SECOND;

  stopwatchProgram.beepIntervalMillis = beepIntervalMillis;
  changeProgram(PROGRAM_STOPWATCH);

  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <meta http-equiv='refresh' content='3;url=/'>\
            <link rel='stylesheet' href='/stylesheet.css'>\
            <style>\
                nav {\
                    background-color: green;\
                    color: white;\
                }\
            </style>\
        </head>\
        <body>\
            <nav><h1>Success</h1></nav>\
            <p><strong>Stopwatch</strong> program activated. Returning to main menu.</p>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgramScoredCountdown() {
  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
    <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <link rel='stylesheet' href='/stylesheet.css'>\
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
            <nav><h1>Activate Scored Countdown</h1></nav>\
            <div><a href='/changeProgram'>Back</a></div>\
            <form method='post' action=''>\
                <div>\
                    <strong>Countdown Time</strong>\
                    <div><label>Minutes <input type='number' name='durationMinutes' placeholder='0' min='0' max='60'></label></div>\
                    <div><label>Seconds <input type='number' name='durationSeconds' placeholder='0' min='0' max='60'></label></div>\
                </div>\
                <div>\
                    <strong>Initial Scores</strong>\
                    <div><label>Left <input type='number' name='leftScore' placeholder='0' min='0' max='99'></label></div>\
                    <div><label>Right <input type='number' name='rightScore' placeholder='0' min='0' max='99'></label></div>\
                </div>\
                <div>\
                    <strong>Preparation Time</strong>\
                    <div><label>Seconds <input type='number' name='readySeconds' placeholder='5' min='0' max='10'></label></div>\
                </div>\
                <input type='submit' value='Activate'>\
            </form>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserChangeProgramScoredCountdownSubmit() {
  int readySeconds = userServer.hasArg(F("readySeconds"))
    ? userServer.arg(F("readySeconds")).toInt()
    : 5;
  readySeconds = constrain(readySeconds, 0, 10);
  unsigned long durationMinutes = userServer.hasArg(F("durationMinutes"))
    ? userServer.arg(F("durationMinutes")).toInt()
    : 0;
  durationMinutes = constrain(durationMinutes, 0, 60);
  unsigned long durationSeconds = userServer.hasArg(F("durationSeconds"))
    ? userServer.arg(F("durationSeconds")).toInt()
    : 0;
  durationSeconds = constrain(durationSeconds, 0, 60);
  int leftScore = userServer.hasArg(F("leftScore"))
    ? userServer.arg(F("leftScore")).toInt()
    : 0;
  leftScore = constrain(leftScore, 0, 99);
  int rightScore = userServer.hasArg(F("rightScore"))
    ? userServer.arg(F("rightScore")).toInt()
    : 0;
  rightScore = constrain(rightScore, 0, 99);

  unsigned long durationMillis = durationMinutes * MILLIS_PER_MINUTE
    + durationSeconds * MILLIS_PER_SECOND;

  scoredCountdownProgram.readySeconds = readySeconds;
  scoredCountdownProgram.durationMillis = durationMillis;
  scoredCountdownProgram.leftScore = leftScore;
  scoredCountdownProgram.rightScore = rightScore;

  changeProgram(PROGRAM_SCORED_COUNTDOWN);

  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock</title>\
            <meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, minimum-scale=1.0'>\
            <link rel='stylesheet' href='/stylesheet.css'>\
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
            <p><strong>Scored Countdown</strong> program activated. Returning to main menu.</p>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserReboot() {
  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock</title>\
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
                <h1>GymClock</h1>\
            </nav>\
            <div>\
                <h2>Confirm Restart</h2>\
                <form action='rebootSubmit' method='post'>\
                    <p>Are you sure you want to reboot the system?<p>\
                    <p>If not, navigate back to the previous page.</p>\
                    <input type='submit' value='Reboot'>\
                </form>\
            </div>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);
}

void serveUserRebootSubmit() {
  WiFiClient client = userServer.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>GymClock</title>\
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
                <h1>GymClock</h1>\
            </nav>\
            <div>\
                <h2>Restarted</h2>\
                <p>Adios, amigo. System going down for reboot NOW!</p>\
            </div>\
        </body>\
    </html>"));
  userServer.send(200, F("text/html"), body);

  Debug.println(F("Rebooting due to user request"));
  delay(100);
  ESP.restart();
}

void serveUserEnableUdpDebug() {
  serveSharedEnableUdpDebug(userServer, F("GymClock"));
}

void serveUserEnableUdpDebugSubmit() {
  serveSharedEnableUdpDebugSubmit(userServer, F("GymClock"));
}

void serveUserDisableUdpDebug() {
  serveSharedDisableUdpDebug(userServer, F("GymClock"));
}

void serveUserDisableUdpDebugSubmit() {
  serveSharedDisableUdpDebugSubmit(userServer, F("GymClock"));
}

void serveUserScoredCountdownIncrementLeft() {
  if (scoredCountdownProgram.leftScore < 99) {
    scoredCountdownProgram.leftScore += 1;
  }
  serveUserIndex();
}

void serveUserScoredCountdownIncrementRight() {
  if (scoredCountdownProgram.rightScore < 99) {
    scoredCountdownProgram.rightScore += 1;
  }
  serveUserIndex();
}

void serveUserScoredCountdownDecrementLeft() {
  if (scoredCountdownProgram.leftScore > 0) {
    scoredCountdownProgram.leftScore -= 1;
  }
  serveUserIndex();
}

void serveUserScoredCountdownDecrementRight() {
  if (scoredCountdownProgram.rightScore > 0) {
    scoredCountdownProgram.rightScore -= 1;
  }
  serveUserIndex();
}

void serveUserDebugDump() {
  Debug.dump();
  serveUserIndex();
}

void serveUserStylesheet() {
  serveSharedStylesheet(userServer);
}

// TODO: Move into shared_ui.h
void serveSharedStylesheet(ESP8266WebServer & server) {
  WiFiClient client = server.client();
  String body = "";
  body.reserve(2048);
  // Remember every line needs to be ended with a backslash!
  body.concat(F("\
    body {\
      background-color: white;\
    }\
  "));
  server.send(200, F("text/css"), body);
}

void updateWiFiSettings(String newSSID, String newPassword) {
  strncpy(wifiSSID, newSSID.c_str(), MAX_SSID_SIZE);
  strncpy(wifiPassword, newPassword.c_str(), MAX_PASSWORD_SIZE);
  byte ssidLen = newSSID.length();
  if (ssidLen > MAX_SSID_SIZE) {
    ssidLen = MAX_SSID_SIZE;
    Debug.println(F("Truncating new SSID to MAX_SSID_SIZE"));
  }
  Debug.print(F("New SSID length is "));
  Debug.println(ssidLen);
  Debug.print(F("New SSID is "));
  Debug.println(wifiSSID);
  byte passwordLen = newPassword.length();
  if (passwordLen > MAX_PASSWORD_SIZE) {
    passwordLen = MAX_PASSWORD_SIZE;
    Debug.println(F("Truncating new password to MAX_PASSWORD_SIZE"));
  }
  Debug.print(F("New password length is "));
  Debug.println(passwordLen);
  Debug.print(F("New password is "));
  Debug.println(wifiPassword);
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

  Debug.println("Disconnecting WiFi.");
  WiFi.disconnect(false);
  if (strlen(wifiSSID) > 0 && strlen(wifiPassword) > 0) {
    Debug.println("Beginning WiFi.");
    WiFi.begin(wifiSSID, wifiPassword);
  }
}
