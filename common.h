#ifndef GYMCLOCK_COMMON_H
#define GYMCLOCK_COMMON_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPUpdateServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <AceTime.h>
#include <IPAddress.h>

const char AP_SSID[] = "GymClockAdmin";
// Change this to the actual password before flashing the sign.
const char AP_PASSWORD[] = "password";
const IPAddress AP_LOCAL_IP(192, 168, 4, 22);
const IPAddress AP_GATEWAY(192, 168, 4, 9);
const IPAddress AP_SUBNET_MASK(255, 255, 255, 0);

// SER
const int srDataPin = D3;
// RCLK
const int srStorageClockPin = D2;
// SRCLK
const int srShiftClockPin = D1;
// The address pins
const int addrBitMap[4] = { D4, D5, D6, D7 };
// Controls Speakers
const int tonePin = D0;

// Indexed by 'char' - '0'
const byte segmentsForNumericChars[] = {
  //_GFEDCBA
  0b00111111, // '0'
  0b00110000, // '1'
  0b01101101, // '2'
  0b01111001, // '3'
  0b01110010, // '4'
  0b01011011, // '5'
  0b01011111, // '6'
  0b00110001, // '7'
  0b01111111, // '8'
  0b01111011, // '9'
};
// Indexed by 'char' - 'a' or 'A'
const byte segmentsForAlphabeticChars[] = {
  //_GFEDCBA
  0b01110111, // 'a'
  0b00000000, // 'b'
  0b00000000, // 'c'
  0b01111100, // 'd'
  0b01001111, // 'e'
  0b00000000, // 'f'
  0b01011111, // 'g'
  0b01110110, // 'h'
  0b00000000, // 'i'
  0b00000000, // 'j'
  0b00000000, // 'k'
  0b00001110, // 'l'
  0b00000000, // 'm'
  0b00110111, // 'n'
  0b00111111, // 'o'
  0b01110011, // 'p'
  0b00000000, // 'q'
  0b01000100, // 'r'
  0b01011011, // 's'
  0b00110001, // 't'
  0b00000000, // 'u'
  0b00000000, // 'v'
  0b00000000, // 'w'
  0b00000000, // 'x'
  0b01110010, // 'y'
  0b00000000, // 'z'
};

extern bool networkActive;

const int NUM_DIGITS = 10;
// 7 for the main segments in the digit, and the last bit for the colon/separator segment.
const int NUM_SEGMENTS_PER_DIGIT = 8;
const int NUM_SEGMENTS = NUM_DIGITS * NUM_SEGMENTS_PER_DIGIT;
// Holds the state for the entire display. The main loop constantly renders the display by lighting
// up digits in turn over and over. Human persistence of vision creates the illusion that the entire
// display is lit up at once.
// The main 6 digits are at indices 0-5. The left 2-digit cluster is at indices 6-7, and the right at 8-9.
extern byte displayState[NUM_DIGITS];
// The number of digits that will be lit at once. 10 would be ideal, but would take too much current (~1 amp per fully-lit digit).
const int LIT_RENDER_WINDOW = 3;

const int ZONE_MGR_CACHE_SIZE = 3;
extern ace_time::BasicZoneManager<ZONE_MGR_CACHE_SIZE> zoneManager;

const unsigned long MILLIS_PER_SECOND = 1000;
const unsigned long MILLIS_PER_MINUTE = 60 * MILLIS_PER_SECOND;
const unsigned long MILLIS_PER_HOUR = 60 * MILLIS_PER_MINUTE;

void waitForSerial();

void renderDisplay();

// Helper function used by the main renderDisplay function. Selects a certain
// digit by setting the address lines appropriately.
void selectDigitForRender(int digit);

void renderToDigit(byte digitState);

// Display an ASCII character on a given digit, optionally lighting up the colon light too.
void updateDigit(byte digit, unsigned char character);

void show2DigitNumber(int number, byte firstDigit);

void clearDisplay();

char * formatIntoTemp(IPAddress val);

char * formatIntoTemp(long val);

char * formatFloatIntoTemp(float val);

void beginWifi();

#endif