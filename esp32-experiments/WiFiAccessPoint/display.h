#ifndef GYMCLOCK_DISPLAY_H
#define GYMCLOCK_DISPLAY_H

#include <stdint.h>

void initializeDisplay();

// Display an ASCII character on a given digit, optionally lighting up the colon light too.
void updateDigit(uint8_t digit, unsigned char character);
void show2DigitNumber(int number, uint8_t firstDigit);
void clearDisplay();

#endif