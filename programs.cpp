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
        case PROGRAM_COUNTDOWN:
            countdownCo.reset();
            if (countdownCo.isSuspended()) {
                countdownCo.resume();
            }
            break;
        case PROGRAM_STOPWATCH:
            stopwatchProgram.reset();
            if (stopwatchProgram.isSuspended()) {
                stopwatchProgram.resume();
            }
        default:
            Serial.println("ERROR: cannot change to unknown program!");
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
    if (exceptProgram != PROGRAM_COUNTDOWN) {
        countdownCo.suspend();
    }
    if (exceptProgram != PROGRAM_STOPWATCH) {
        stopwatchProgram.suspend();
    }
}

int TestProgramCo::runCoroutine() {
    COROUTINE_LOOP() {
        Serial.println("Test program showing TEST");
        for (int i = 0; i < NUM_DIGITS; i++) {
            updateDigit(i, ' ');
        }
        updateDigit(1, 'T');
        updateDigit(2, 'E');
        updateDigit(3, 'S');
        updateDigit(4, 'T');

        tone(tonePin, NOTE_C4, 500);
        COROUTINE_DELAY(750);
        tone(tonePin, NOTE_E4, 500);
        COROUTINE_DELAY(750);
        tone(tonePin, NOTE_G4, 500);
        COROUTINE_DELAY(1500);

        Serial.println("Test program showing 0-9");
        for (int i = 0; i < NUM_DIGITS; i++) {
            updateDigit(i, '0' + i);
        }

        COROUTINE_DELAY(2000);

        Serial.println("Test program showing all 8s");
        for (int i = 0; i < NUM_DIGITS; i++) {
            updateDigit(i, '8');
        }

        COROUTINE_DELAY(2000);
    }
}

int InitProgramCo::runCoroutine() {
    COROUTINE_BEGIN();

    clearDisplay();
    updateDigit(0, 'H');
    updateDigit(1, 'E');
    updateDigit(2, 'L');
    updateDigit(3, 'L');
    updateDigit(4, 'O');

    COROUTINE_DELAY(5000);

    if (!networkActive) {
        clearDisplay();
        updateDigit(0, 'N');
        updateDigit(1, 'O');
        updateDigit(3, 'N');
        updateDigit(4, 'E');
        updateDigit(5, 'T');
        COROUTINE_AWAIT(networkActive);
    }

    Serial.println("Net on in InitProgramCo");
    clearDisplay();
    updateDigit(0, 'N');
    updateDigit(1, 'E');
    updateDigit(2, 'T');
    updateDigit(4, 'O');
    updateDigit(5, 'N');
    COROUTINE_DELAY(3000);

    changeProgram(PROGRAM_CLOCK);

    COROUTINE_END();
}

int ClockProgramCo::runCoroutine() {
    COROUTINE_LOOP() {
        if (!networkActive) {
            clearDisplay();
            updateDigit(0, 'N');
            updateDigit(1, 'O');
            updateDigit(3, 'N');
            updateDigit(4, 'T');
            updateDigit(5, 'P');
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

int CountdownCo::runCoroutine() {
    COROUTINE_BEGIN();

    clearDisplay();
    show2DigitNumber(this->sets, 6);
    updateDigit(0, 'G');
    updateDigit(1, 'E');
    updateDigit(2, 'T');
    COROUTINE_DELAY(1000);
    updateDigit(0, 'R');
    updateDigit(1, 'E');
    updateDigit(2, 'A');
    updateDigit(3, 'D');
    updateDigit(4, 'Y');
    COROUTINE_DELAY(2000);

    // Do an initial countdown before the first set.
    while (this->readySeconds > 0) {
        clearDisplay();
        show2DigitNumber(this->readySeconds, 2);
        show2DigitNumber(this->sets, 6);

        // Also beep the last 3 seconds.
        if (this->readySeconds <= 3) {
            // TODO: play sound here
        }

        COROUTINE_DELAY(1000);
        this->readySeconds -= 1;
    }

    // Count down the sets.
    while (this->sets > 0) {
        // TODO: play sound to mark the start of a set.

        this->startMillis = millis();
        this->ageMillis = 0;

        // Count down the set duration.
        while (this->ageMillis < this->setDurationMillis) {
            unsigned long remainingMillis = this->setDurationMillis - this->ageMillis;
            unsigned long hoursPart = remainingMillis / MILLIS_PER_HOUR;
            remainingMillis -= hoursPart * MILLIS_PER_HOUR;
            unsigned long minutesPart = remainingMillis / MILLIS_PER_MINUTE;
            remainingMillis -= minutesPart * MILLIS_PER_MINUTE;
            unsigned long secondsPart = remainingMillis / MILLIS_PER_SECOND;
            remainingMillis -= secondsPart * MILLIS_PER_SECOND;

            clearDisplay();
            show2DigitNumber(hoursPart, 0);
            show2DigitNumber(minutesPart, 2);
            show2DigitNumber(secondsPart, 4);
            show2DigitNumber(this->sets, 6);


            COROUTINE_DELAY(250);

            unsigned long now = millis();
            // Just make sure the clock never goes backwards. This could probably never happen
            // on a single-core machine anyways but can't hurt to guard against it.
            if (now < startMillis) {
                now = startMillis;
            }
            this->ageMillis = now - this->startMillis;
        }

        this->sets -= 1;

        // Now show "REST" on the display for 3 seconds, followed by counting down the
        // rest timer. We could easily account the 3-second period against the rest time by
        // updating the startMillis before we show the word, but I choose not to because I
        // think its more understandable not to. Plus, 3 extra seconds of rest :)
        if (this->sets > 0 && this->restDurationMillis > 0) {
            // TODO: play sound too
            clearDisplay();
            updateDigit(1, 'R');
            updateDigit(2, 'E');
            updateDigit(3, 'S');
            updateDigit(4, 'T');
            show2DigitNumber(this->sets, 6);
            updateDigit(8, '-');
            updateDigit(9, '-');

            COROUTINE_DELAY(3000);

            this->startMillis = millis();
            this->ageMillis = 0;

            // Count down the rest duration.
            while (this->ageMillis < this->restDurationMillis) {
                unsigned long remainingMillis = this->restDurationMillis - this->ageMillis;
                unsigned long hoursPart = remainingMillis / MILLIS_PER_HOUR;
                remainingMillis -= hoursPart * MILLIS_PER_HOUR;
                unsigned long minutesPart = remainingMillis / MILLIS_PER_MINUTE;
                remainingMillis -= minutesPart * MILLIS_PER_MINUTE;
                unsigned long secondsPart = remainingMillis / MILLIS_PER_SECOND;
                remainingMillis -= secondsPart * MILLIS_PER_SECOND;

                clearDisplay();
                show2DigitNumber(hoursPart, 0);
                show2DigitNumber(minutesPart, 2);
                show2DigitNumber(secondsPart, 4);
                show2DigitNumber(this->sets, 6);
                updateDigit(8, '-');
                updateDigit(9, '-');

                COROUTINE_DELAY(250);

                unsigned long now = millis();
                // Just make sure the clock never goes backwards. This could probably never happen
                // on a single-core machine anyways but can't hurt to guard against it.
                if (now < startMillis) {
                    now = startMillis;
                }
                this->ageMillis = now - this->startMillis;
            }
        }
    }

    static int i;
    for (i = 0; i < 10; i++) {
        clearDisplay();
        updateDigit(1, 'D');
        updateDigit(2, 'O');
        updateDigit(3, 'N');
        updateDigit(4, 'E');

        COROUTINE_DELAY(1000);
        clearDisplay();
        COROUTINE_DELAY(750);
    }

    COROUTINE_END();
}

int StopwatchProgram::runCoroutine() {
    COROUTINE_BEGIN();

    clearDisplay();

    unsigned long now = millis();
    this->startMillis = now;
    this->lastBeepTimeMillis = now;

    while (true) {
        now = millis();
        unsigned long age = now > this->startMillis ? now - this->startMillis : 0;
        unsigned long hoursPart = age / MILLIS_PER_HOUR;
        age -= hoursPart * MILLIS_PER_HOUR;
        unsigned long minutesPart = age / MILLIS_PER_MINUTE;
        age -= minutesPart * MILLIS_PER_MINUTE;
        unsigned long secondsPart = age / MILLIS_PER_SECOND;
        age -= secondsPart * MILLIS_PER_SECOND;
        unsigned long decisecondsPart = age / 10;

        // Loop around once we exceed 99 hours.
        if (hoursPart > 99) {
            this->startMillis = millis();
            continue;
        }

        // beepIntervalMillis == 0 disables the beep functionality.
        if (this->beepIntervalMillis > 0) {
            unsigned long millisSinceLastBeep = now > this->lastBeepTimeMillis ? now - this->lastBeepTimeMillis : 0;
            if (millisSinceLastBeep > this->beepIntervalMillis) {
                // TODO: play beep sound
            }
        }

        // If the stopwatch has recorded over an hour, then show HH:MM:SS, otherwise show
        // MM:SS:DS.
        if (hoursPart > 0) {
            show2DigitNumber(hoursPart, 0);
            show2DigitNumber(minutesPart, 2);
            show2DigitNumber(secondsPart, 4);
            COROUTINE_DELAY(250);
        } else {
            show2DigitNumber(minutesPart, 0);
            show2DigitNumber(secondsPart, 2);
            show2DigitNumber(decisecondsPart, 4);
            COROUTINE_DELAY(75);
        }
    }

    COROUTINE_END();
}