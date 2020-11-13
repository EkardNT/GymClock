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
#include "user_ui.h"

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

  setupUserUI();
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
