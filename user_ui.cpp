#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "user_ui.h"
#include "shared_ui.h"
#include "debug.h"
#include "programs.h"
#include "common.h"

extern ESP8266WebServer userServer;

void serveUserIndex() {
    Debug.println(F("serveUserIndex"));

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
                    <div $ENABLE_UDP_DEBUG><a href='/enableUdpDebug'>Enable UDP Debug</a></div>\
                    <div $DISABLE_UDP_DEBUG><a href='/disableUdpDebug'>Disable UDP Debug</a></div>\
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
    Debug.println(F("serveUserChangeProgram"));

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
    Debug.println(F("serveUserChangeProgramClock"));

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
    Debug.println(F("serveUserChangeProgramClockSubmit"));

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
    Debug.println(F("serveUserChangeProgramTest"));

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
    Debug.println(F("serveUserChangeProgramTestSubmit"));

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
    Debug.println(F("serveUserChangeProgramCountdown"));

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
    Debug.println(F("serveUserChangeProgramCountdownSubmit"));

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
    Debug.println(F("serveUserChangeProgramStopwatch"));

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
    Debug.println(F("serveUserChangeProgramStopwatchSubmit"));

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
    Debug.println(F("serveUserChangeProgramScoredCountdown"));

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
    Debug.println(F("serveUserChangeProgramScoredCountdownSubmit"));

    // TODO: hasArg returns true for ""!
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

    Debug.printf("Setting ScoredCountdown settings to readySeconds=%d, durationMillis=%d, leftScore=%d, rightScore=%d\r\n",
        readySeconds, durationMillis, leftScore, rightScore);
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
    Debug.println(F("serveUserReboot"));

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
    Debug.println(F("serveUserRebootSubmit"));

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
    Debug.println(F("serveUserEnableUdpDebug"));
    serveSharedEnableUdpDebug(userServer, F("GymClock"));
}

void serveUserEnableUdpDebugSubmit() {
    Debug.println(F("serveUserEnableUdpDebugSubmit"));
    serveSharedEnableUdpDebugSubmit(userServer, F("GymClock"));
}

void serveUserDisableUdpDebug() {
    Debug.println(F("serveUserDisableUdpDebug"));
    serveSharedDisableUdpDebug(userServer, F("GymClock"));
}

void serveUserDisableUdpDebugSubmit() {
    Debug.println(F("serveUserDisableUdpDebugSubmit"));
    serveSharedDisableUdpDebugSubmit(userServer, F("GymClock"));
}

void serveUserScoredCountdownIncrementLeft() {
    Debug.println(F("serveUserScoredCountdownIncrementLeft"));
    if (scoredCountdownProgram.leftScore < 99) {
        scoredCountdownProgram.leftScore += 1;
    }
    serveUserIndex();
}

void serveUserScoredCountdownIncrementRight() {
    Debug.println(F("serveUserScoredCountdownIncrementRight"));
    if (scoredCountdownProgram.rightScore < 99) {
        scoredCountdownProgram.rightScore += 1;
    }
    serveUserIndex();
}

void serveUserScoredCountdownDecrementLeft() {
    Debug.println(F("serveUserScoredCountdownDecrementLeft"));
    if (scoredCountdownProgram.leftScore > 0) {
        scoredCountdownProgram.leftScore -= 1;
    }
    serveUserIndex();
}

void serveUserScoredCountdownDecrementRight() {
    Debug.println(F("serveUserScoredCountdownDecrementRight"));
    if (scoredCountdownProgram.rightScore > 0) {
        scoredCountdownProgram.rightScore -= 1;
    }
    serveUserIndex();
}

void serveUserDebugDump() {
    Debug.println(F("serveUserDebugDump"));
    Debug.dump();
    serveUserIndex();
}

void serveUserStylesheet() {
    Debug.println(F("serveUserStylesheet"));
    serveSharedStylesheet(userServer);
}

void setupUserUI() {
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
}