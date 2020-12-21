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
#include "settings.h"

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

  initSettings();
  dumpSettings();

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP_STA);

  gotIpEventHandler = WiFi.onStationModeGotIP(networkInit);
  disconnectedEventHandler = WiFi.onStationModeDisconnected(networkStop);

  WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY, AP_SUBNET_MASK);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(true);

  beginWifi();

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

// Called both from the setup() method as well as from the Admin UI /changeWiFi handler.
void beginWifi() {
  char const * wifiSsid = getWifiSsid();
  char const * wifiPassword = getWifiPassword();

  if (strlen(wifiSsid) > 0 && strlen(wifiPassword) > 0) {
    Debug.printf("Trying to log on to WiFi network %s in station mode\n", wifiSsid);
    if (getEnableStaticIp()) {
      IPAddress staticIp = getStaticIp();
      IPAddress gatewayIp = getGatewayIp();
      IPAddress subnetMask = getSubnetMask();
      Debug.print("Configuring static IP Address ");
      Debug.println(staticIp);
      WiFi.config(staticIp, gatewayIp, subnetMask);
    }
    WiFi.begin(wifiSsid, wifiPassword);
  } else {
    Debug.println("Not logging onto network because WiFi SSID and/or password from settings is empty");
  }
}