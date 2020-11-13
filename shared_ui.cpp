#include <IPAddress.h>
#include "shared_ui.h"
#include "debug.h"

void serveSharedEnableUdpDebug(ESP8266WebServer & server, const __FlashStringHelper * title) {
  WiFiClient client = server.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>$TITLE</title>\
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
                <h1>$TITLE</h1>\
            </nav>\
            <div>\
                <h2>Enable UDP Debug</h2>\
                <form action='' method='post'>\
                    <p>Enter the IP Address and port that the UDP listener is bound to.<p>\
                    <div><label>IP <input type='text' name='debugIp' placeholder='192.168.0.XXX' required></label></div>\
                    <div><label>Port <input type='text' name='debugPort' placeholder='1024-65535' required></label></div>\
                    <input type='submit' value='Enable'>\
                </form>\
            </div>\
        </body>\
    </html>"));
  body.replace(F("$TITLE"), title);
  server.send(200, F("text/html"), body);
}

void serveSharedEnableUdpDebugSubmit(ESP8266WebServer & server, const __FlashStringHelper * title) {
  String debugIpText = server.arg(F("debugIp"));
  IPAddress debugIp;
  if (!debugIp.fromString(debugIpText)) {
    server.send(400, F("text/plain"), F("Invalid IP"));
    return;
  }
  int debugPort = server.arg(F("debugPort")).toInt();
  debugPort = constrain(debugPort, 1024, 65535);

  Debug.enableUdp(debugIp, debugPort);
  Debug.println("UDP Debug enabled");

  WiFiClient client = server.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>$TITLE</title>\
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
                <h1>$TITLE</h1>\
            </nav>\
            <div>\
                <h2>Success</h2>\
                <p>UDP debug has been enabled.</p>\
            </div>\
        </body>\
    </html>"));
  body.replace(F("$TITLE"), title);
  server.send(200, F("text/html"), body);
}

void serveSharedDisableUdpDebug(ESP8266WebServer & server, const __FlashStringHelper * title) {
  WiFiClient client = server.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>$TITLE</title>\
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
                <h1>$TITLE</h1>\
            </nav>\
            <div>\
                <h2>Disable UDP Debug</h2>\
                <form action='' method='post'>\
                    <p>If you click Disable below then UDP debug will be disabled.<p>\
                    <input type='submit' value='Disable'>\
                </form>\
            </div>\
        </body>\
    </html>"));
  server.send(200, F("text/html"), body);
}

void serveSharedDisableUdpDebugSubmit(ESP8266WebServer & server, const __FlashStringHelper * title) {
  Debug.disableUdp();
  Debug.println("UDP Debug disabled");

  WiFiClient client = server.client();
  String body = "";
  body.reserve(2048);
  body.concat(F("\
      <html>\
        <head>\
            <title>$TITLE</title>\
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
                <h1>$TITLE</h1>\
            </nav>\
            <div>\
                <h2>Success</h2>\
                <p>UDP debug has been disabled.</p>\
            </div>\
        </body>\
    </html>"));
  body.replace(F("$TITLE"), title);
  server.send(200, F("text/html"), body);
}