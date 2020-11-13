#ifndef GYMCLOCK_SHARED_UI_H
#define GYMCLOCK_SHARED_UI_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

void serveSharedStylesheet(ESP8266WebServer & server);
void serveSharedEnableUdpDebug(ESP8266WebServer & server, const __FlashStringHelper * title);
void serveSharedEnableUdpDebugSubmit(ESP8266WebServer & server, const __FlashStringHelper * title);
void serveSharedDisableUdpDebug(ESP8266WebServer & server, const __FlashStringHelper * title);
void serveSharedDisableUdpDebugSubmit(ESP8266WebServer & server, const __FlashStringHelper * title);

#endif