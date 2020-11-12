#ifndef GYMCLOCK_PROGRAMS_H
#define GYMCLOCK_PROGRAMS_H

#include <AceRoutine.h>

const int PROGRAM_INIT = 0;
const int PROGRAM_TEST = 1;
const int PROGRAM_CLOCK = 2;
const int PROGRAM_COUNTDOWN = 3;
const int PROGRAM_STOPWATCH = 4;
const int PROGRAM_SCORED_COUNTDOWN = 5;

int currentProgram();
void changeProgram(int newProgram);
void suspendAll(int exceptProgram);

class TestProgram : public ace_routine::Coroutine {
    public:
        TestProgram() {}
        int runCoroutine() override;
};

class InitProgram : public ace_routine::Coroutine {
    public:
        InitProgram() {}
        int runCoroutine() override;
};

class ClockProgram : public ace_routine::Coroutine {
    public:
        ClockProgram() {}
        int runCoroutine() override;

        bool format24 = false;
};

class CountdownProgram : public ace_routine::Coroutine {
    public:
        CountdownProgram() {}
        int runCoroutine() override;

        int readySeconds = 0;
        int sets = 0;
        unsigned long setDurationMillis = 0;
        unsigned long restSeconds = 0;

    private:
        unsigned long startMillis = 0;
        unsigned long ageMillis = 0;
};

class StopwatchProgram : public ace_routine::Coroutine {
    public:
        StopwatchProgram() {}
        int runCoroutine() override;

        unsigned long beepIntervalMillis = 0;

    private:
        unsigned long startMillis = 0;
        unsigned long lastBeepTimeMillis = 0;
};

class ScoredCountdownProgram : public ace_routine::Coroutine {
    public:
        ScoredCountdownProgram() {}
        int runCoroutine() override;
        int leftScore = 0;
        int rightScore = 0;
        int readySeconds = 0;
        unsigned long durationMillis;
};

extern TestProgram testProgram;
extern InitProgram initProgram;
extern ClockProgram clockProgram;
extern CountdownProgram countdownProgram;
extern StopwatchProgram stopwatchProgram;
extern ScoredCountdownProgram scoredCountdownProgram;

#endif