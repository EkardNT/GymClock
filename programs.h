#ifndef GYMCLOCK_PROGRAMS_H
#define GYMCLOCK_PROGRAMS_H

#include <AceRoutine.h>

const int PROGRAM_INIT = 0;
const int PROGRAM_TEST = 1;
const int PROGRAM_CLOCK = 2;
const int PROGRAM_COUNTDOWN = 3;
const int PROGRAM_STOPWATCH = 4;

int currentProgram();
void changeProgram(int newProgram);
void suspendAll(int exceptProgram);

class TestProgramCo : public ace_routine::Coroutine {
    public:
        TestProgramCo() {}
        int runCoroutine() override;
};

class InitProgramCo : public ace_routine::Coroutine {
    public:
        InitProgramCo() {}
        int runCoroutine() override;
};

class ClockProgramCo : public ace_routine::Coroutine {
    public:
        ClockProgramCo() {}
        int runCoroutine() override;

        bool format24 = false;
};

class CountdownCo : public ace_routine::Coroutine {
    public:
        CountdownCo() {}
        int runCoroutine() override;

        int readySeconds = 0;
        int sets = 0;
        unsigned long setDurationMillis = 0;
        unsigned long restDurationMillis = 0;

    private:
        unsigned long startMillis = 0;
        unsigned long ageMillis = 0;
};

class StopwatchProgram : public ace_routine::Coroutine {
    public:
        StopwatchProgram() {}
        int runCoroutine() override;

    private:
        unsigned long startMillis = 0;
};

extern TestProgramCo testProgramCo;
extern InitProgramCo initProgramCo;
extern ClockProgramCo clockProgramCo;
extern CountdownCo countdownCo;
extern StopwatchProgram stopwatchProgram;

#endif