#include <NTPClient.h>
#include <AceTime.h>
#include "programs.h"
#include "common.h"

using namespace ace_routine;

const int NOTE_C4 = 262;
const int NOTE_E4 = 330;
const int NOTE_G4 = 392;

extern NTPClient timeClient;

int currProgram = PROGRAM_INIT;

int currentProgram() {
    return currProgram;
}

void changeProgram(int newProgram) {
    currProgram = newProgram;
    suspendAll(newProgram);
    switch (newProgram) {
        case PROGRAM_INIT:
            initProgramCo.reset();
            if (initProgramCo.isSuspended()) {
                initProgramCo.resume();
            }
            break;
        case PROGRAM_TEST:
            testProgramCo.reset();
            if (testProgramCo.isSuspended()) {
                testProgramCo.resume();
            }
            break;
        case PROGRAM_CLOCK:
            clockProgramCo.reset();
            if (clockProgramCo.isSuspended()) {
                clockProgramCo.resume();
            }
            break;
    }
}

void suspendAll(int exceptProgram) {
    if (exceptProgram != PROGRAM_INIT) {
        initProgramCo.suspend();
    }
    if (exceptProgram != PROGRAM_TEST) {
        testProgramCo.suspend();
    }
    if (exceptProgram != PROGRAM_CLOCK) {
        clockProgramCo.suspend();
    }
}

int TestProgramCo::runCoroutine() {
    COROUTINE_LOOP() {
        Serial.println("Test program showing TEST");
        for (int i = 0; i < NUM_DIGITS; i++) {
            updateDigit(i, ' ', false);
        }
        updateDigit(1, 'T', false);
        updateDigit(2, 'E', false);
        updateDigit(3, 'S', false);
        updateDigit(4, 'T', false);

        tone(tonePin, NOTE_C4, 500);
        COROUTINE_DELAY(750);
        tone(tonePin, NOTE_E4, 500);
        COROUTINE_DELAY(750);
        tone(tonePin, NOTE_G4, 500);
        COROUTINE_DELAY(1500);

        Serial.println("Test program showing 0-9");
        for (int i = 0; i < NUM_DIGITS; i++) {
            updateDigit(i, '0' + i, false);
        }

        COROUTINE_DELAY(2000);

        Serial.println("Test program showing all 8s");
        for (int i = 0; i < NUM_DIGITS; i++) {
            updateDigit(i, '8', false);
        }

        COROUTINE_DELAY(2000);
    }
}

int InitProgramCo::runCoroutine() {
    COROUTINE_BEGIN();

    clearDisplay();
    updateDigit(0, 'H', false);
    updateDigit(1, 'E', false);
    updateDigit(2, 'L', false);
    updateDigit(3, 'L', false);
    updateDigit(4, 'O', false);

    COROUTINE_DELAY(5000);

    if (!networkActive) {
        clearDisplay();
        updateDigit(0, 'N', false);
        updateDigit(1, 'O', false);
        updateDigit(3, 'N', false);
        updateDigit(4, 'E', false);
        updateDigit(5, 'T', false);
        COROUTINE_AWAIT(networkActive);
    }

    Serial.println("Net on in InitProgramCo");
    clearDisplay();
    updateDigit(0, 'N', false);
    updateDigit(1, 'E', false);
    updateDigit(2, 'T', false);
    updateDigit(4, 'O', false);
    updateDigit(5, 'N', false);
    COROUTINE_DELAY(3000);

    Serial.println("Transition to clock program in InitProgramCo");
    // Transition to clock program.
    clockProgramCo.resume();

    COROUTINE_END();
}

int ClockProgramCo::runCoroutine() {
    COROUTINE_LOOP() {
        if (!networkActive) {
            clearDisplay();
            updateDigit(0, 'N', false);
            updateDigit(1, 'O', false);
            updateDigit(3, 'N', false);
            updateDigit(4, 'T', false);
            updateDigit(5, 'P', false);
            COROUTINE_AWAIT(networkActive);
        }

        unsigned long unixEpochSeconds = timeClient.getEpochTime();
        ace_time::TimeZone localTz = zoneManager.createForZoneInfo(&ace_time::zonedb::kZoneAmerica_Los_Angeles);
        ace_time::ZonedDateTime localTime = ace_time::ZonedDateTime::forUnixSeconds(unixEpochSeconds, localTz);

        uint8_t hour = localTime.hour();

        if (hour > 12 && !this->format24) {
            hour -= 12;
        }

        clearDisplay();
        show2DigitNumber(hour, 0);
        show2DigitNumber(localTime.minute(), 2);
        show2DigitNumber(localTime.second(), 4);
        show2DigitNumber(localTime.month(), 6);
        show2DigitNumber(localTime.day(), 8);

        COROUTINE_DELAY(250);
    }
}

void printlnStatus(Coroutine *co) {
    if (co->isSuspended()) {
        Serial.println("suspended");
    } else if (co->isYielding()) {
        Serial.println("yielding");
    } else if (co->isDelaying()) {
        Serial.println("delaying");
    } else if (co->isRunning()) {
        Serial.println("running");
    } else if (co->isEnding()) {
        Serial.println("ending");
    } else if (co->isTerminated()) {
        Serial.println("terminated");
    }
}

int LoopProgramCo::runCoroutine() {
    COROUTINE_LOOP() {
        Serial.println("Loop coroutine");
        CoroutineScheduler::list(Serial);
        Serial.println("Coroutine statuses:");
        Serial.print("testProgramCo: ");
        printlnStatus(&testProgramCo);
        Serial.print("initProgramCo: ");
        printlnStatus(&initProgramCo);
        Serial.print("clockProgramCo: ");
        printlnStatus(&clockProgramCo);
        Serial.print("loopProgramCo: ");
        printlnStatus(&loopProgramCo);
        COROUTINE_DELAY(2000);
    }
}

