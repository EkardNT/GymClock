#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include "i2c_protocol.h"

// SER
const int srDataPin = 32;
// RCLK
const int srStorageClockPin = 33; // TODO
// SRCLK
const int srShiftClockPin = 25; // TODO
// The address pins
const int addrBitMap[4] = { 26, 27, 12, 13 };

const int NUM_DIGITS = 10;
// 7 for the main segments in the digit, and the last bit for the colon/separator segment.
const int NUM_SEGMENTS_PER_DIGIT = 8;
const int NUM_SEGMENTS = NUM_DIGITS * NUM_SEGMENTS_PER_DIGIT;
// Holds the state for the entire display. The main loop constantly renders the display by lighting
// up digits in turn over and over. Human persistence of vision creates the illusion that the entire
// display is lit up at once.
// The main 6 digits are at indices 0-5. The left 2-digit cluster is at indices 6-7, and the right at 8-9.
volatile uint8_t displayState[NUM_DIGITS];
// The number of digits that will be lit at once. 10 would be ideal, but would take too much current (~1 amp per fully-lit digit).
const int LIT_RENDER_WINDOW = 3;

// Helper function used by the main renderDisplay function. Selects a certain
// digit by setting the address lines appropriately.
void selectDigitForRender(int digit) {
    for (int i = 0; i < 4; i++) {
        if ((digit >> i) & 1 == 1) {
            digitalWrite(addrBitMap[i], HIGH);
        } else {
            digitalWrite(addrBitMap[i], LOW);
        }
    }
}

// Actually write to the shift register on the selected digit controller.
void renderToDigit(byte digitState) {
    digitalWrite(srStorageClockPin, LOW);
    shiftOut(srDataPin, srShiftClockPin, MSBFIRST, digitState);
    digitalWrite(srStorageClockPin, HIGH);
}

void renderDisplay() {
    for (int digit = 0; digit < NUM_DIGITS + LIT_RENDER_WINDOW; digit++) {
        // Turn off digit that has fallen out of the lit window.
        int offDigit = digit - LIT_RENDER_WINDOW;
        if (offDigit >= 0 && offDigit < NUM_DIGITS) {
            selectDigitForRender(offDigit);
            renderToDigit(0);
        }

        // Turn on digit that has entered the lit window.
        if (digit < NUM_DIGITS) {
            selectDigitForRender(digit);
            renderToDigit(displayState[digit]);
        }
    }
}

// the setup function runs once when you press reset or power the board
void setup() {
    pinMode(srDataPin, OUTPUT);
    pinMode(srStorageClockPin, OUTPUT);
    pinMode(srShiftClockPin, OUTPUT);
    pinMode(addrBitMap[0], OUTPUT);
    pinMode(addrBitMap[1], OUTPUT);
    pinMode(addrBitMap[2], OUTPUT);
    pinMode(addrBitMap[3], OUTPUT);

    digitalWrite(srDataPin, LOW);
    digitalWrite(srStorageClockPin, LOW);
    digitalWrite(srShiftClockPin, LOW);
    digitalWrite(addrBitMap[0], LOW);
    digitalWrite(addrBitMap[1], LOW);
    digitalWrite(addrBitMap[2], LOW);
    digitalWrite(addrBitMap[3], LOW);

    for (int i = 0; i < NUM_DIGITS; i++) {
        updateDigit(i, '0' + i);
    }
}

// the loop function runs over and over again forever
void loop() {
    renderDisplay();
}
