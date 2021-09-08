#include <Arduino.h>
#include <stdint.h>

#include "display.h"

// SER
const int srDataPin = 32;
// RCLK
const int srStorageClockPin = 33; // TODO
// SRCLK
const int srShiftClockPin = 25; // TODO
// The address pins
const int addrBitMap[4] = { 26, 27, 14, 12 };

// Indexed by 'char' - '0'
const uint8_t segmentsForNumericChars[] = {
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
const uint8_t segmentsForAlphabeticChars[] = {
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

TaskHandle_t renderTaskHandle;
StaticSemaphore_t displayStateMutex;
SemaphoreHandle_t displayStateMutexHandle;

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

void renderToDigit(byte digitState) {
    digitalWrite(srStorageClockPin, LOW);
    shiftOut(srDataPin, srShiftClockPin, MSBFIRST, digitState);
    digitalWrite(srStorageClockPin, HIGH);
}

void renderDisplay() {
    xSemaphoreTakeRecursive(displayStateMutexHandle, portMAX_DELAY);
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
    xSemaphoreGiveRecursive(displayStateMutexHandle);
}

void renderLoop(void * unused) {
    while (true) {
        renderDisplay();
        // Need a delay to allow the framework's watchdog task to run,
        // otherwise the program will abort() (aka reboot) with an error
        // saying "Task watchdog got triggered. The following tasks did not reset the watchdog in time".
        delay(1);
    }
}

void initializeDisplay() {
    displayStateMutexHandle = xSemaphoreCreateMutexStatic(&displayStateMutex);

    pinMode(srDataPin, OUTPUT);
    pinMode(srStorageClockPin, OUTPUT);
    pinMode(srShiftClockPin, OUTPUT);
    pinMode(addrBitMap[0], OUTPUT);
    pinMode(addrBitMap[1], OUTPUT);
    pinMode(addrBitMap[2], OUTPUT);
    pinMode(addrBitMap[3], OUTPUT);

    for (int i = 0; i < NUM_DIGITS; i++) {
        updateDigit(0, '0' + i);
    }

    xTaskCreatePinnedToCore(
        renderLoop, /* Function to implement the task */
        "renderLoop", /* Name of the task */
        10000,  /* Stack size in words */
        NULL,  /* Task input parameter */
        0,  /* Priority of the task */
        &renderTaskHandle,  /* Task handle. */
        0); /* Core where the task should run */
}

// Display an ASCII character on a given digit, optionally lighting up the colon light too.
void updateDigit(uint8_t digit, unsigned char character) {
    xSemaphoreTakeRecursive(displayStateMutexHandle, portMAX_DELAY);
    uint8_t segments = 0;
    if (character >= '0' && character <= '9') {
        segments = segmentsForNumericChars[character - '0'];
    } else if (character >= 'A' && character <= 'Z') {
        segments = segmentsForAlphabeticChars[character - 'A'];
    } else if (character >= 'a' && character <= 'z') {
        segments = segmentsForAlphabeticChars[character - 'a'];
    } else if (character == '-') {
        segments = 0b01000000;
    }
    displayState[digit] = segments;
    xSemaphoreGiveRecursive(displayStateMutexHandle);
}

void show2DigitNumber(int number, uint8_t firstDigit) {
    xSemaphoreTakeRecursive(displayStateMutexHandle, portMAX_DELAY);
    if (number > 99) {
        updateDigit(firstDigit, '-');
        updateDigit(firstDigit + 1, '-');
        return;
    }
    if (number < 0) {
        updateDigit(firstDigit, '-');
        updateDigit(firstDigit + 1, '-');
        return;
    }
    updateDigit(firstDigit, '0' + number / 10);
    updateDigit(firstDigit + 1, '0' + number % 10);
    xSemaphoreGiveRecursive(displayStateMutexHandle);
}

void clearDisplay() {
    xSemaphoreTakeRecursive(displayStateMutexHandle, portMAX_DELAY);
    for (int i = 0; i < NUM_DIGITS; i++) {
        updateDigit(i, ' ');
    }
    xSemaphoreGiveRecursive(displayStateMutexHandle);
}