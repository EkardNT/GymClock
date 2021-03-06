#include "common.h"

const int FORMAT_BUF_SIZE = 256;
char tempFormatBuffer[FORMAT_BUF_SIZE] = {0};

void waitForSerial() {
  // Wait for any data before proceeding...
  while (Serial.available() == 0) {
    yield();
  }
  // Then discard that data.
  while (Serial.available() > 0) {
    Serial.read();
  }
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

// Display an ASCII character on a given digit, optionally lighting up the colon light too.
void updateDigit(byte digit, unsigned char character) {
  byte segments = 0;
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
}

void show2DigitNumber(int number, byte firstDigit) {
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
}

void clearDisplay() {
  for (int i = 0; i < NUM_DIGITS; i++) {
    updateDigit(i, ' ');
  }
}

char * formatIntoTemp(IPAddress val) {
  snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%d.%d.%d.%d", val[0], val[1], val[2], val[3]);
  return tempFormatBuffer;
}

char * formatIntoTemp(long val) {
  snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%d", val);
  return tempFormatBuffer;
}

char * formatFloatIntoTemp(float val) {
  snprintf(tempFormatBuffer, FORMAT_BUF_SIZE, "%.2f", val);
  return tempFormatBuffer;
}