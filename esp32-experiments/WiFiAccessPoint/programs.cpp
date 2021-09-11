#include <AceRoutine.h>
#include <Arduino.h>
#include "programs.h"

COROUTINE(blinkLed) {
    COROUTINE_LOOP() {
        digitalWrite(LED_BUILTIN, HIGH);
        COROUTINE_DELAY(100);
        digitalWrite(LED_BUILTIN, LOW);
        COROUTINE_DELAY(500);
    }
}

void initializePrograms() {
    pinMode(LED_BUILTIN, OUTPUT);
}

void updatePrograms() {
    blinkLed.runCoroutine();
}
